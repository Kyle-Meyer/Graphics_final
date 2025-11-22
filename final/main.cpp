#include "filesystem_support/file_locator.hpp"
#include "geometry/geometry.hpp"
#include "scene/graphics.hpp"
#include "scene/scene.hpp"

#include "final/multi_texture_shader_node.hpp"
#include "final/bump_mapping_shader_node.hpp"
#include "scene/sphere_section.hpp"
#include "scene/color_node.hpp"
#include "scene/presentation_node.hpp"
#include "scene/light_node.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace cg
{

void logmsg(const char *message, ...)
{
    static FILE *lfile = NULL;
    if(lfile == NULL) { lfile = fopen("Module11.log", "w"); }

    va_list arg;
    va_start(arg, message);
    vfprintf(lfile, message, arg);
    putc('\n', lfile);
    fflush(lfile);
    va_end(arg);
}

} // namespace cg

// SDL Objects
SDL_Window       *g_sdl_window = nullptr;
SDL_GLContext     g_gl_context;
constexpr int32_t DRAWS_PER_SECOND = 60;
constexpr int32_t DRAW_INTERVAL_MILLIS =
    static_cast<int32_t>(1000.0 / static_cast<double>(DRAWS_PER_SECOND));

// Scene graph root and state
std::shared_ptr<cg::SceneNode> g_scene_root;
std::shared_ptr<cg::CameraNode> g_camera;
std::shared_ptr<cg::MultiTextureShaderNode> g_multi_tex_shader;
std::shared_ptr<cg::BumpMappingShaderNode> g_bump_shader;

cg::SceneState g_scene_state;

// Bump mapping controls
float g_bump_strength = 1.0f;

// Camera controls
bool    g_animate = false;
bool    g_forward = true;
float   g_velocity = 1.0f;
int32_t g_mouse_x = 0;
int32_t g_mouse_y = 0;
int32_t g_render_width = 800;
int32_t g_render_height = 600;

// Multi-texture controls
cg::BlendMode g_current_blend_mode = cg::BlendMode::MIX;
float         g_mix_factor = 0.5f;
int           g_active_textures = 2; // How many textures to use (1-4)

void sleep(int32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_scene_state.init();
    g_scene_root->draw(g_scene_state);

    SDL_GL_SwapWindow(g_sdl_window);
}

void reshape(int32_t width, int32_t height)
{
    g_render_width = width;
    g_render_height = height;
    glViewport(0, 0, width, height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    g_camera->change_aspect_ratio(aspect);
}

void update_view(int32_t x, int32_t y, bool forward)
{
    float dx = 4.0f * ((x - (static_cast<float>(g_render_width * 0.5f))) /
                       static_cast<float>(g_render_width));
    float dy = 4.0f * (((static_cast<float>(g_render_height * 0.5f) - y)) /
                       static_cast<float>(g_render_height));
    float dz = (forward) ? g_velocity : -g_velocity;
    g_camera->move_and_turn(dx * g_velocity, dy * g_velocity, dz);
}

void print_current_mode()
{
    std::cout << "\n=== Multi-Texture Settings ===\n";
    std::cout << "Active Textures: " << g_active_textures << "\n";
    std::cout << "Blend Mode: ";
    switch(g_current_blend_mode)
    {
        case cg::BlendMode::MIX:      std::cout << "MIX (Linear Interpolation)\n"; break;
        case cg::BlendMode::MULTIPLY: std::cout << "MULTIPLY\n"; break;
        case cg::BlendMode::ADD:      std::cout << "ADD\n"; break;
        case cg::BlendMode::SUBTRACT: std::cout << "SUBTRACT\n"; break;
    }
    if(g_current_blend_mode == cg::BlendMode::MIX)
    {
        std::cout << "Mix Factor: " << g_mix_factor << "\n";
    }
    std::cout << "==============================\n";
}

void cycle_blend_mode()
{
    int mode = static_cast<int>(g_current_blend_mode);
    mode = (mode + 1) % 4;
    g_current_blend_mode = static_cast<cg::BlendMode>(mode);
    g_multi_tex_shader->set_blend_mode(g_current_blend_mode);
    print_current_mode();
}

void adjust_mix_factor(float delta)
{
    g_mix_factor += delta;
    if(g_mix_factor < 0.0f) g_mix_factor = 0.0f;
    if(g_mix_factor > 1.0f) g_mix_factor = 1.0f;
    g_multi_tex_shader->set_mix_factor(g_mix_factor);
    print_current_mode();
}

void toggle_texture(int unit)
{
    if(unit < 0 || unit >= 4) return;
    
    // Toggle the texture
    static bool enabled[4] = {true, true, false, false};
    enabled[unit] = !enabled[unit];
    g_multi_tex_shader->set_texture_enabled(unit, enabled[unit]);
    
    // Count active textures
    g_active_textures = 0;
    for(int i = 0; i < 4; i++)
    {
        if(enabled[i]) g_active_textures++;
    }
    
    std::cout << "Texture " << unit << " " << (enabled[unit] ? "ENABLED" : "DISABLED") << "\n";
    print_current_mode();
}

bool handle_key_event(const SDL_Event &event)
{
    bool cont_program = true;
    bool upper_case = (event.key.mod & SDL_KMOD_SHIFT) || (event.key.mod & SDL_KMOD_CAPS);

    switch(event.key.key)
    {
        case SDLK_ESCAPE:
            cont_program = false;
            break;

        // Camera reset
        case SDLK_I:
            g_camera->set_position(cg::Point3(0.0f, -80.0f, 40.0f));
            g_camera->set_look_at_pt(cg::Point3(0.0f, 0.0f, 20.0f));
            g_camera->set_view_up(cg::Vector3(0.0f, 0.0f, 1.0f));
            break;

        // Camera controls
        case SDLK_R:
            g_camera->roll(upper_case ? -5 : 5);
            break;
        case SDLK_P:
            g_camera->pitch(upper_case ? -5 : 5);
            break;
        case SDLK_H:
            g_camera->heading(upper_case ? -5 : 5);
            break;

        // Blend mode cycling
        case SDLK_B:
            cycle_blend_mode();
            break;

        // Mix factor adjustment
        case SDLK_M:
            adjust_mix_factor(upper_case ? 0.1f : -0.1f);
            break;

        // Toggle individual textures
        case SDLK_1:
            toggle_texture(0);
            break;
        case SDLK_2:
            toggle_texture(1);
            break;
        case SDLK_3:
            toggle_texture(2);
            break;
        case SDLK_4:
            toggle_texture(3);
            break;

        // Print current settings
        case SDLK_SPACE:
            print_current_mode();
            break;

        // Bump strength adjustment
        case SDLK_N:
            g_bump_strength += upper_case ? 0.2f : -0.2f;
            if (g_bump_strength < 0.0f) g_bump_strength = 0.0f;
            if (g_bump_strength > 3.0f) g_bump_strength = 3.0f;
            if (g_bump_shader) g_bump_shader->set_bump_strength(g_bump_strength);
            std::cout << "Bump strength: " << g_bump_strength << "\n";
            break;
    }

    return cont_program;
}

void handle_mouse_event(const SDL_Event &event)
{
    switch(event.button.button)
    {
        case 1: // Left button
            g_forward = true;
            g_animate = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            break;
        case 3: // Right button
            g_forward = false;
            g_animate = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            break;
    }
}

void handle_mouse_motion_event(const SDL_Event &event)
{
    g_mouse_x = event.motion.x;
    g_mouse_y = event.motion.y;
}

bool handle_window_event(const SDL_Event &event)
{
    if(event.type == SDL_EVENT_WINDOW_RESIZED || 
       event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
        reshape(event.window.data1, event.window.data2);
    }
    return true;
}

bool handle_events()
{
    SDL_Event event;
    bool      cont_program = true;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_EVENT_QUIT:
                cont_program = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                cont_program = handle_key_event(event);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                handle_mouse_event(event);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                handle_mouse_motion_event(event);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                cont_program = handle_window_event(event);
                break;
        }
    }
    return cont_program;
}

void construct_scene()
{
    // Create multi-texture shader
    g_multi_tex_shader = std::make_shared<cg::MultiTextureShaderNode>();
    if(!g_multi_tex_shader->create("multi_texture.vert", "multi_texture.frag"))
    {
        std::cout << "Failed to create multi-texture shader\n";
        exit(-1);
    }
    if(!g_multi_tex_shader->get_locations())
    {
        std::cout << "Failed to get shader locations\n";
        exit(-1);
    }

    // Load textures
    cg::ImageData tex0, tex1, tex2, tex3;
    load_image_data(tex0, "floor_tiles.jpg", false);
    load_image_data(tex1, "grainy_wood.jpg", false);
    load_image_data(tex2, "earthp2.jpg", false);
    load_image_data(tex3, "dogs_poker.jpg", false);

    // Bind textures to shader
    g_multi_tex_shader->bind_texture(0, &tex0);
    g_multi_tex_shader->bind_texture(1, &tex1);
    g_multi_tex_shader->bind_texture(2, &tex2);
    g_multi_tex_shader->bind_texture(3, &tex3);

    // Set initial blend mode
    g_multi_tex_shader->set_blend_mode(g_current_blend_mode);
    g_multi_tex_shader->set_mix_factor(g_mix_factor);

    // Initially enable only first two textures
    g_multi_tex_shader->set_texture_enabled(0, true);
    g_multi_tex_shader->set_texture_enabled(1, true);
    g_multi_tex_shader->set_texture_enabled(2, false);
    g_multi_tex_shader->set_texture_enabled(3, false);

    // Free image data after upload
    free_image_data(tex0);
    free_image_data(tex1);
    free_image_data(tex2);
    free_image_data(tex3);

    // Create camera
    g_camera = std::make_shared<cg::CameraNode>();
    g_camera->set_position(cg::Point3(0.0f, -80.0f, 40.0f));
    g_camera->set_look_at_pt(cg::Point3(0.0f, 0.0f, 20.0f));
    g_camera->set_view_up(cg::Vector3(0.0f, 0.0f, 1.0f));
    g_camera->set_perspective(45.0f, 800.0f / 600.0f, 1.0f, 500.0f);

    // Get shader attribute locations using public getter methods
    int pos_loc = g_multi_tex_shader->get_position_loc();
    int norm_loc = g_multi_tex_shader->get_normal_loc();
    int tex_loc = g_multi_tex_shader->get_texcoord_loc();

    // Create geometry - use UnitSquareSurface (note: constructor takes n first, then locations, then tex_scale)
    auto unit_square = std::make_shared<cg::UnitSquareSurface>(10, pos_loc, norm_loc, tex_loc, 10.0f);

    // Create multiple objects to show multi-texturing
    
    // Central large plane
    auto center_plane_transform = std::make_shared<cg::TransformNode>();
    center_plane_transform->scale(40.0f, 40.0f, 1.0f);
    center_plane_transform->translate(0.0f, 0.0f, 0.0f);

    // Left wall
    auto left_wall_transform = std::make_shared<cg::TransformNode>();
    left_wall_transform->rotate_y(90.0f);
    left_wall_transform->scale(40.0f, 40.0f, 1.0f);
    left_wall_transform->translate(-40.0f, 0.0f, 20.0f);

    // Right wall
    auto right_wall_transform = std::make_shared<cg::TransformNode>();
    right_wall_transform->rotate_y(-90.0f);
    right_wall_transform->scale(40.0f, 40.0f, 1.0f);
    right_wall_transform->translate(40.0f, 0.0f, 20.0f);

    // Back wall
    auto back_wall_transform = std::make_shared<cg::TransformNode>();
    back_wall_transform->rotate_x(-90.0f);
    back_wall_transform->scale(40.0f, 40.0f, 1.0f);
    back_wall_transform->translate(0.0f, 40.0f, 20.0f);

    // Floating cube
    auto cube_transform = std::make_shared<cg::TransformNode>();
    cube_transform->translate(0.0f, 0.0f, 25.0f);
    cube_transform->rotate_z(45.0f);
    cube_transform->rotate_x(30.0f);
    cube_transform->scale(15.0f, 15.0f, 15.0f);

    // Construct scene graph with camera at top (shared by all shaders)
    g_scene_root = std::make_shared<cg::SceneNode>();
    g_scene_root->add_child(g_camera);

    // Multi-texture shader branch
    g_camera->add_child(g_multi_tex_shader);

    // Add geometry to multi-texture shader
    g_multi_tex_shader->add_child(center_plane_transform);
    center_plane_transform->add_child(unit_square);

    g_multi_tex_shader->add_child(left_wall_transform);
    left_wall_transform->add_child(unit_square);

    g_multi_tex_shader->add_child(right_wall_transform);
    right_wall_transform->add_child(unit_square);

    g_multi_tex_shader->add_child(back_wall_transform);
    back_wall_transform->add_child(unit_square);

    g_multi_tex_shader->add_child(cube_transform);
    cube_transform->add_child(unit_square);

    // =====================================================
    // BUMP MAPPING SECTION - Red sphere with bump map
    // =====================================================
    std::cout << "\nSetting up bump-mapped sphere...\n";

    // Create bump mapping shader
    g_bump_shader = std::make_shared<cg::BumpMappingShaderNode>();
    if (!g_bump_shader->create("bump_mapping.vert", "bump_mapping.frag"))
    {
        std::cout << "Failed to create bump mapping shader\n";
        exit(-1);
    }
    if (!g_bump_shader->get_locations())
    {
        std::cout << "Failed to get bump shader locations\n";
        exit(-1);
    }

    // Set global ambient light
    g_bump_shader->set_global_ambient(cg::Color4(0.2f, 0.2f, 0.2f, 1.0f));
    g_bump_shader->set_bump_strength(g_bump_strength);

    // Load bump/normal map texture
    cg::ImageData normal_map;
    load_image_data(normal_map, "bumper.jpg", false);
    if (normal_map.data)
    {
        g_bump_shader->bind_normal_map(&normal_map);
        free_image_data(normal_map);
    }
    else
    {
        std::cout << "Warning: Could not load bumper.jpg normal map\n";
    }

    // Create a light for the bump-mapped objects
    auto bump_light = std::make_shared<cg::LightNode>(0);
    bump_light->enable();
    bump_light->set_position(cg::HPoint3(50.0f, -50.0f, 80.0f, 1.0f));  // Point light
    bump_light->set_ambient(cg::Color4(0.1f, 0.1f, 0.1f, 1.0f));
    bump_light->set_diffuse(cg::Color4(1.0f, 1.0f, 1.0f, 1.0f));
    bump_light->set_specular(cg::Color4(1.0f, 1.0f, 1.0f, 1.0f));
    bump_light->set_attenuation(1.0f, 0.0f, 0.0f);  // No distance attenuation

    // Create red glossy material for the sphere (use PresentationNode for full material support)
    auto red_material = std::make_shared<cg::PresentationNode>(
        cg::Color4(0.2f, 0.0f, 0.0f, 1.0f),   // ambient
        cg::Color4(0.8f, 0.1f, 0.1f, 1.0f),   // diffuse
        cg::Color4(1.0f, 1.0f, 1.0f, 1.0f),   // specular
        cg::Color4(0.0f, 0.0f, 0.0f, 1.0f),   // emission
        64.0f                                   // shininess
    );

    // Get bump shader attribute locations
    int bump_pos_loc = g_bump_shader->get_position_loc();
    int bump_norm_loc = g_bump_shader->get_normal_loc();
    int bump_tex_loc = g_bump_shader->get_texcoord_loc();
    int bump_tan_loc = g_bump_shader->get_tangent_loc();
    int bump_bitan_loc = g_bump_shader->get_bitangent_loc();

    // Create unit square geometry with tangent space (for testing bump mapping on flat surface)
    auto bump_square = std::make_shared<cg::UnitSquareSurface>(
        10,  // subdivisions
        bump_pos_loc, bump_norm_loc, bump_tex_loc,
        bump_tan_loc, bump_bitan_loc
    );

    // Transform for the square - position it to the right of the cube, facing camera
    auto square_transform = std::make_shared<cg::TransformNode>();
    square_transform->translate(35.0f, 0.0f, 25.0f);
    square_transform->rotate_x(90.0f);  // Rotate to face camera (camera is at -Y)
    square_transform->scale(25.0f, 25.0f, 1.0f);

    // Build bump mapping branch (shares camera with multi-tex branch):
    // g_camera -> g_bump_shader -> bump_light -> red_material -> square_transform -> bump_square
    g_camera->add_child(g_bump_shader);
    g_bump_shader->add_child(bump_light);
    bump_light->add_child(red_material);
    red_material->add_child(square_transform);
    square_transform->add_child(bump_square);

    std::cout << "Bump-mapped red square added to scene!\n";
    std::cout << "Press N/n to adjust bump strength\n\n";
}

int main(int argc, char **argv)
{
    cg::set_root_paths(argv[0]);

    // Print controls
    std::cout << "\n====================================\n";
    std::cout << "  MULTI-TEXTURING & BUMP MAPPING DEMO\n";
    std::cout << "====================================\n\n";
    std::cout << "CAMERA CONTROLS:\n";
    std::cout << "  i       - Reset to initial view\n";
    std::cout << "  R/r     - Roll camera\n";
    std::cout << "  P/p     - Pitch camera\n";
    std::cout << "  H/h     - Change heading\n";
    std::cout << "  Mouse   - Click and drag to navigate\n\n";
    std::cout << "MULTI-TEXTURE CONTROLS:\n";
    std::cout << "  b       - Cycle blend modes (MIX/MULTIPLY/ADD/SUBTRACT)\n";
    std::cout << "  M/m     - Increase/decrease mix factor\n";
    std::cout << "  1-4     - Toggle textures 0-3\n";
    std::cout << "  SPACE   - Print current settings\n\n";
    std::cout << "BUMP MAPPING CONTROLS:\n";
    std::cout << "  N/n     - Increase/decrease bump strength\n\n";
    std::cout << "  ESC     - Exit\n";
    std::cout << "====================================\n\n";

    // Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, 
                          "Multi-Texturing Demo - Kyle Meyer");
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 800);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 600);

    g_sdl_window = SDL_CreateWindowWithProperties(props);
    if(!g_sdl_window)
    {
        std::cout << "Error creating window: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Create OpenGL context
    g_gl_context = SDL_GL_CreateContext(g_sdl_window);
    std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

#if BUILD_WINDOWS
    if(glewInit() != GLEW_OK)
    {
        std::cout << "GLEW initialization failed\n";
        exit(1);
    }
#endif

    // OpenGL setup
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Construct scene
    construct_scene();
    
    reshape(800, 600);
    print_current_mode();

    // Main loop
    while(handle_events())
    {
        if(g_animate) update_view(g_mouse_x, g_mouse_y, g_forward);
        display();
        sleep(DRAW_INTERVAL_MILLIS);
    }

    // Cleanup
    SDL_GL_DestroyContext(g_gl_context);
    SDL_DestroyWindow(g_sdl_window);
    SDL_Quit();

    return 0;
}
