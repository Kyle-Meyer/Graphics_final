//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    Module9/main.cpp
//	Purpose: OpenGL and SDL program to draw a simple 3-D scene. It starts
//           with some simple object modeling and representation, adds camera
//           and projection controls, adds lighting and shading, then adds
//           texture mapping.
//
//============================================================================

#include "filesystem_support/file_locator.hpp"
#include "geometry/geometry.hpp"
#include "scene/graphics.hpp"
#include "scene/scene.hpp"

#include "Module9/lighting_shader_node.hpp"
#include "Module9/shader_src.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace cg
{

// Simple logging function, should be defined in the cg namespace
void logmsg(const char *message, ...)
{
    // Open file if not already opened
    static FILE *lfile = NULL;
    if(lfile == NULL) { lfile = fopen("Module9.log", "w"); }

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
constexpr int32_t DRAWS_PER_SECOND = 72;
constexpr int32_t DRAW_INTERVAL_MILLIS =
    static_cast<int32_t>(1000.0 / static_cast<double>(DRAWS_PER_SECOND));

// Root of the scene graph and scene state
std::shared_ptr<cg::SceneNode> g_scene_root;

// Global camera node (so we can change view)
std::shared_ptr<cg::CameraNode> g_camera;

// Keep the spotlight global so we can update its poisition
std::shared_ptr<cg::LightNode> Spotlight;

cg::SceneState g_scene_state;

// While mouse button is down, the view will be updated
bool    g_animate = false;
bool    g_forward = true;
float   g_velocity = 1.0f;
int32_t g_mouse_x = 0;
int32_t g_mouse_y = 0;
int32_t g_render_width = 800;
int32_t g_render_height = 600;

// Sleep function to help run a reasonable timer
void sleep(int32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/**
 * Display callback. Clears the prior scene and draws a new one.
 */
void display()
{
    // Clear the framebuffer and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Init scene state and draw the scene graph
    g_scene_state.init();
    g_scene_root->draw(g_scene_state);

    // Swap buffers
    SDL_GL_SwapWindow(g_sdl_window);
}

/**
 * Reshape callback. Update projection to reflect new aspect ratio.
 * @param  width  Window width
 * @param  height Window height
 */
void reshape(int32_t width, int32_t height)
{
    g_render_width = width;
    g_render_height = height;

    // Reset the viewport
    glViewport(0, 0, width, height);

    // Reset the perspective projection to reflect the change of aspect ratio
    // Make sure we cast to float so we get a fractional aspect ratio.
    g_camera->change_aspect_ratio(static_cast<float>(width) / static_cast<float>(height));
}

void update_spotlight()
{
    cg::Point3 pos = g_camera->get_position();
    Spotlight->set_position(cg::HPoint3(pos.x, pos.y, pos.z, 1.0f));
    cg::Vector3 dir = g_camera->get_view_plane_normal() * -1.0f;
    Spotlight->set_spotlight_direction(dir);
}

/**
 * Updates the view given the mouse position and whether to move
 * forward or backward.
 * @param  x        Window x position.
 * @param  y        Window y position.
 * @param  forward  Forward flag (true if moving forward).
 */
void update_view(int32_t x, int32_t y, bool forward)
{
    // Find relative dx and dy relative to center of the window
    float dx = 4.0f * ((x - (static_cast<float>(g_render_width * 0.5f))) /
                       static_cast<float>(g_render_width));
    float dy = 4.0f * (((static_cast<float>(g_render_height * 0.5f) - y)) /
                       static_cast<float>(g_render_height));
    float dz = (forward) ? g_velocity : -g_velocity;
    g_camera->move_and_turn(dx * g_velocity, dy * g_velocity, dz);
    update_spotlight();
}

/**
 * Mouse handler (called when a mouse button state changes)
 */
void handle_mouse_event(const SDL_Event &event)
{
    switch(event.button.button)
    {
        // On a left button up event move_and_turn the view with forward motion
        case 1:
            g_forward = true;
            if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) g_animate = true;
            // Disable animation when the button is released
            else g_animate = false;
            break;
            // On a right button up event move_and_turn the view with reverse motion
        case 3:
            g_forward = false;
            if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) g_animate = true;
            // Disable animation when the button is released
            else g_animate = false;
            break;
    }
}

/**
 * Mouse motion callback (called when mouse button is depressed)
 */
void handle_mouse_motion_event(const SDL_Event &event)
{
    // Update position used for changing the view and force a new view
    g_mouse_x = event.motion.x;
    g_mouse_y = event.motion.y;
}

/**
 * Window event handler.
 */
bool handle_window_event(const SDL_Event &event)
{
    bool cont_program = true;

    switch(event.type)
    {
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            reshape(event.window.data1, event.window.data2);
            break;
        default: break;
    }

    return cont_program;
}

/**
 * Keyboard event handler.
 */
bool handle_key_event(const SDL_Event &event)
{
    bool cont_program = true;
    bool upper_case = (event.key.mod & SDL_KMOD_SHIFT) || (event.key.mod & SDL_KMOD_CAPS);

    switch(event.key.key)
    {
        case SDLK_ESCAPE: cont_program = false; break;

        // Reset the view
        case SDLK_I:
            g_camera->set_position(cg::Point3(0.0f, -100.0f, 20.0f));
            g_camera->set_look_at_pt(cg::Point3(0.0f, 0.0f, 20.0f));
            g_camera->set_view_up(cg::Vector3(0.0f, 0.0f, 1.0f));
            update_spotlight();
            break;

        // roll the camera by 5 degrees
        case SDLK_R:
            if(upper_case) g_camera->roll(-5);
            else g_camera->roll(5);
            update_spotlight();
            break;

        // Change the pitch of the camera by 5 degrees
        case SDLK_P:
            if(upper_case) g_camera->pitch(-5);
            else g_camera->pitch(5);
            update_spotlight();
            break;

        // Change the heading of the camera by 5 degrees
        case SDLK_H:
            if(upper_case) g_camera->heading(-5);
            else g_camera->heading(5);
            update_spotlight();
            break;

        // Go faster/slower
        case SDLK_V:
            if(upper_case) g_velocity += 0.2f;
            else
            {
                g_velocity -= 0.2f;
                if(g_velocity < 0.2f) g_velocity = 0.1f;
            }
            break;

        // Slide right/left
        case SDLK_X:
            if(upper_case) g_camera->slide(5.0f, 0.0f, 0.0f);
            else g_camera->slide(-5.0f, 0.0f, 0.0f);
            update_spotlight();
            break;

        // Slide up/down
        case SDLK_Y:
            if(upper_case) g_camera->slide(0.0f, 5.0f, 0.0f);
            else g_camera->slide(0.0f, -5.0f, 0.0f);
            update_spotlight();
            break;

        // Move forward/backward
        case SDLK_F:
            if(upper_case) g_camera->slide(0.0f, 0.0f, -5.0f);
            else g_camera->slide(0.0f, 0.0f, 5.0f);
            update_spotlight();
            break;
        default: break;
    }

    return cont_program;
}

/**
 * Handle Events function.
 */
bool handle_events()
{
    SDL_Event e;
    bool      cont_program = true;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: cont_program = false; break;

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: cont_program = handle_window_event(e); break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: handle_mouse_event(e); break;

            case SDL_EVENT_MOUSE_MOTION: handle_mouse_motion_event(e); break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: cont_program = handle_key_event(e); break;
            default: break;
        }
    }
    return cont_program;
}

/**
 * Convenience method to add a material, then a transform, then a
 * geometry node as a child to a specified parent node.
 * @param  parent    Parent scene node.
 * @param  material  Presentation node.
 * @param  transform Transformation node.
 * @param  geometry  Geometry node.
 */
void add_sub_tree(std::shared_ptr<cg::SceneNode> parent,
                  std::shared_ptr<cg::SceneNode> material,
                  std::shared_ptr<cg::SceneNode> transform,
                  std::shared_ptr<cg::SceneNode> geometry)
{
    parent->add_child(material);
    material->add_child(transform);
    transform->add_child(geometry);
}

/**
 * Construct room as a child of the specified node
 * @param  unit_square  Geometry node to use
 * @return Returns a scene node that describes the room.
 */
std::shared_ptr<cg::SceneNode> construct_room(std::shared_ptr<cg::UnitSquareSurface> unit_square)
{
    // Contruct transform nodes for the walls. Perform rotations so the
    // walls face inwards
    auto floor_transform = std::make_shared<cg::TransformNode>();
    floor_transform->scale(200.0f, 200.0f, 1.0f);

    // Back wall is rotated +90 degrees about x: (y -> z)
    auto backwall_transform = std::make_shared<cg::TransformNode>();
    backwall_transform->translate(0.0f, 100.0f, 40.0f);
    backwall_transform->rotate_x(90.0f);
    backwall_transform->scale(200.0f, 80.0f, 1.0f);

    // Front wall is rotated -90 degrees about x: (z -> y)
    auto frontwall_transform = std::make_shared<cg::TransformNode>();
    frontwall_transform->translate(0.0f, -100.0f, 40.0f);
    frontwall_transform->rotate_x(-90.0f);
    frontwall_transform->scale(200.0f, 80.0f, 1.0f);

    // Left wall is rotated 90 degrees about y: (z -> x)
    auto leftwall_transform = std::make_shared<cg::TransformNode>();
    leftwall_transform->translate(-100.0f, 0.0f, 40.0f);
    leftwall_transform->rotate_y(90.0f);
    leftwall_transform->scale(80.0f, 200.0f, 1.0f);

    // Right wall is rotated -90 about y: (z -> -x)
    auto rightwall_transform = std::make_shared<cg::TransformNode>();
    rightwall_transform->translate(100.0f, 0.0f, 40.0f);
    rightwall_transform->rotate_y(-90.0f);
    rightwall_transform->scale(80.0f, 200.0f, 1.0f);

    // Ceiling is rotated 180 about x so it faces inwards
    auto ceiling_transform = std::make_shared<cg::TransformNode>();
    ceiling_transform->translate(0.0f, 0.0f, 80.0f);
    ceiling_transform->rotate_x(180.0f);
    ceiling_transform->scale(200.0f, 200.0f, 1.0f);

    // Floor should be tan, mostly dull
    auto floor_material = std::make_shared<cg::PresentationNode>(cg::Color4(0.15f, 0.22f, 0.05f),
                                                                 cg::Color4(0.3f, 0.45f, 0.1f),
                                                                 cg::Color4(0.1f, 0.1f, 0.1f),
                                                                 cg::Color4(0.0f, 0.0f, 0.0f),
                                                                 5.0f);

    // Make the walls reddish, slightly shiny
    auto wall_material = std::make_shared<cg::PresentationNode>(cg::Color4(0.35f, 0.225f, 0.275f),
                                                                cg::Color4(0.7f, 0.55f, 0.55f),
                                                                cg::Color4(0.4f, 0.4f, 0.4f),
                                                                cg::Color4(0.0f, 0.0f, 0.0f),
                                                                16.0f);

    // Ceiling should be white, moderately shiny
    auto ceiling_material = std::make_shared<cg::PresentationNode>(cg::Color4(0.75f, 0.75f, 0.75f),
                                                                   cg::Color4(1.0f, 1.0f, 1.0f),
                                                                   cg::Color4(0.9f, 0.9f, 0.9f),
                                                                   cg::Color4(0.0f, 0.0f, 0.0f),
                                                                   64.0f);

    // Walls. We can group these all under a single presentation node.
    auto room = std::make_shared<cg::SceneNode>();
    room->add_child(wall_material);
    wall_material->add_child(backwall_transform);
    backwall_transform->add_child(unit_square);
    wall_material->add_child(leftwall_transform);
    leftwall_transform->add_child(unit_square);
    wall_material->add_child(rightwall_transform);
    rightwall_transform->add_child(unit_square);
    wall_material->add_child(frontwall_transform);
    frontwall_transform->add_child(unit_square);

    // Add floor and ceiling to the parent. Use convenience method to add
    // presentation, then transform, then geometry.
    add_sub_tree(room, floor_material, floor_transform, unit_square);
    add_sub_tree(room, ceiling_material, ceiling_transform, unit_square);

    return room;
}

/**
 * Construct table
 * @param  box  Geometry node to use for table top
 * @param  leg  Geometry node to use for legs
 * @return Returns a scene node representing the table
 */
std::shared_ptr<cg::SceneNode> construct_table(std::shared_ptr<cg::SceneNode>    box,
                                               std::shared_ptr<cg::ConicSurface> leg)
{
    // Table legs (relative to center of table)
    auto lfleg_transform = std::make_shared<cg::TransformNode>();
    lfleg_transform->translate(-20.0f, -10.0f, 10.0f);
    lfleg_transform->scale(6.0f, 6.0f, 20.0f);
    auto lrleg_transform = std::make_shared<cg::TransformNode>();
    lrleg_transform->translate(-20.0f, 10.0f, 10.0f);
    lrleg_transform->scale(6.0f, 6.0f, 20.0f);
    auto rfleg_transform = std::make_shared<cg::TransformNode>();
    rfleg_transform->translate(20.0f, -10.0f, 10.0f);
    rfleg_transform->scale(6.0f, 6.0f, 20.0f);
    auto rrleg_transform = std::make_shared<cg::TransformNode>();
    rrleg_transform->translate(20.0f, 10.0f, 10.0f);
    rrleg_transform->scale(6.0f, 6.0f, 20.0f);

    // Construct dimensions for the table top
    auto top_transform = std::make_shared<cg::TransformNode>();
    top_transform->translate(0.0f, 0.0f, 23.0f);
    top_transform->scale(60.0f, 30.0f, 6.0f);

    // Create the tree
    auto table = std::make_shared<cg::SceneNode>();
    table->add_child(top_transform);
    top_transform->add_child(box);
    table->add_child(lfleg_transform);
    lfleg_transform->add_child(leg);
    table->add_child(rfleg_transform);
    rfleg_transform->add_child(leg);
    table->add_child(lrleg_transform);
    lrleg_transform->add_child(leg);
    table->add_child(rrleg_transform);
    rrleg_transform->add_child(leg);

    return table;
}

/**
 * Construct a unit box with outward facing normals.
 * @param  unit_square  Geometry node to use
 */
std::shared_ptr<cg::SceneNode>
    construct_unit_box(std::shared_ptr<cg::UnitSquareSurface> unit_square)
{
    // Contruct transform nodes for the sides of the box.
    // Perform rotations so the sides face outwards

    // Bottom is rotated 180 degrees so it faces outwards
    auto bottom_transform = std::make_shared<cg::TransformNode>();
    bottom_transform->translate(0.0f, 0.0f, -0.5f);
    bottom_transform->rotate_x(180.0f);

    // Back is rotated -90 degrees about x: (z -> y)
    auto back_transform = std::make_shared<cg::TransformNode>();
    back_transform->translate(0.0f, 0.5f, 0.0f);
    back_transform->rotate_x(-90.0f);

    // Front wall is rotated 90 degrees about x: (y -> z)
    auto front_transform = std::make_shared<cg::TransformNode>();
    front_transform->translate(0.0f, -0.5f, 0.0f);
    front_transform->rotate_x(90.0f);

    // Left wall is rotated -90 about y: (z -> -x)
    auto left_transform = std::make_shared<cg::TransformNode>();
    left_transform->translate(-0.5f, 0.0f, 00.0f);
    left_transform->rotate_y(-90.0f);

    // Right wall is rotated 90 degrees about y: (z -> x)
    auto right_transform = std::make_shared<cg::TransformNode>();
    right_transform->translate(0.5f, 0.0f, 0.0f);
    right_transform->rotate_y(90.0f);

    // Top
    auto top_transform = std::make_shared<cg::TransformNode>();
    top_transform->translate(0.0f, 0.0f, 0.50f);

    // Create a SceneNode and add the 6 sides of the box.
    auto box = std::make_shared<cg::SceneNode>();
    box->add_child(back_transform);
    back_transform->add_child(unit_square);
    box->add_child(left_transform);
    left_transform->add_child(unit_square);
    box->add_child(right_transform);
    right_transform->add_child(unit_square);
    box->add_child(front_transform);
    front_transform->add_child(unit_square);
    box->add_child(bottom_transform);
    bottom_transform->add_child(unit_square);
    box->add_child(top_transform);
    top_transform->add_child(unit_square);

    return box;
}

/**
 * Construct vase using a surface of revolution.
 */
std::shared_ptr<cg::SceneNode> construct_vase(const int position_loc, const int normal_loc)
{
    // Profile curve. Unit width and height, centered at the center of the vase
    std::vector<cg::Point3> v = {{0.0f, 0.0f, -0.5f},
                                 {0.4f, 0.0f, -0.5f},
                                 {0.6f, 0.0f, -0.45f},
                                 {0.72f, 0.0f, -0.37f},
                                 {0.81f, 0.0f, -0.26f},
                                 {0.82f, 0.0f, -0.18f},
                                 {0.79f, 0.0f, -0.08f},
                                 {0.7f, 0.0f, 0.02f},
                                 {0.55f, 0.0f, 0.13f},
                                 {0.48f, 0.0f, 0.25f},
                                 {0.51f, 0.0f, 0.35f},
                                 {0.53f, 0.0f, 0.41f},
                                 {0.62f, 0.0f, 0.45f},
                                 {0.62f, 0.0f, 0.5f},
                                 {0.65f, 0.0f, 0.5f},
                                 {0.0f, 0.0f, 0.5f}};

    auto surf = std::make_shared<cg::SurfaceOfRevolution>(v, 36, position_loc, normal_loc);

    // Vase color and position
    auto vase_material = std::make_shared<cg::PresentationNode>(cg::Color4(0.35f, 0.15f, 0.25f),
                                                                cg::Color4(0.95f, 0.35f, 0.65f),
                                                                cg::Color4(0.4f, 0.4f, 0.4f),
                                                                cg::Color4(0.0f, 0.0f, 0.0f),
                                                                16.0f);

    auto vase_transform = std::make_shared<cg::TransformNode>();
    vase_transform->translate(0.0f, 75.0f, 10.0f);
    vase_transform->scale(10.0f, 10.0f, 20.0f);

    // Form scene graph
    auto vase = std::make_shared<cg::SceneNode>();
    add_sub_tree(vase, vase_material, vase_transform, surf);
    return vase;
}

/**
 * Construct a sphere with a shiny blue material.
 */
std::shared_ptr<cg::SceneNode> construct_shiny_sphere(int32_t position_loc, int32_t normal_loc)
{
    auto sphere = std::make_shared<cg::SphereSection>(
        -90.0f, 90.0f, 18, -180.0f, 180.0f, 36, 1.0f, position_loc, normal_loc);

    // Shiny blue
    auto shiny_blue = std::make_shared<cg::PresentationNode>(cg::Color4(0.05f, 0.05f, 0.2f),
                                                             cg::Color4(0.2f, 0.2f, 0.7f),
                                                             cg::Color4(1.0f, 1.0f, 1.0f),
                                                             cg::Color4(0.0f, 0.0f, 0.0f),
                                                             85.0f);

    // Sphere
    auto sphere_transform = std::make_shared<cg::TransformNode>();
    sphere_transform->translate(80.0f, 20.0f, 10.0f);
    sphere_transform->scale(10.0f, 10.0f, 10.0f);

    // Form scene graph
    auto shiny_sphere = std::make_shared<cg::SceneNode>();
    add_sub_tree(shiny_sphere, shiny_blue, sphere_transform, sphere);
    return shiny_sphere;
}

/**
 * Construct a sphere with a shiny blue material.
 */
std::shared_ptr<cg::SceneNode> construct_shiny_torus(int32_t position_loc, int32_t normal_loc)
{
    auto torus = std::make_shared<cg::TorusSurface>(20.0f, 5.0f, 36, 18, position_loc, normal_loc);

    // Shiny black
    auto shiny_black = std::make_shared<cg::PresentationNode>(cg::Color4(0.0f, 0.0f, 0.0f),
                                                              cg::Color4(0.01f, 0.01f, 0.01f),
                                                              cg::Color4(0.5f, 0.5f, 0.5f),
                                                              cg::Color4(0.0f, 0.0f, 0.0f),
                                                              32.0f);

    // Torus transform
    auto torus_transform = std::make_shared<cg::TransformNode>();
    torus_transform->translate(0.0f, 95.0f, 23.0f);
    torus_transform->rotate_x(70.0f);

    // Form scene graph
    auto shiny_torus = std::make_shared<cg::SceneNode>();
    add_sub_tree(shiny_torus, shiny_black, torus_transform, torus);
    return shiny_torus;
}

/**
 * Construct lighting for this scene. Note that it is hard coded
 * into the shader node for this exercise.
 * @param  lighting  Pointer to the lighting shader node.
 */
void construct_lighting(std::shared_ptr<cg::LightingShaderNode> lighting)
{
    // Set the global light ambient
    cg::Color4 global_ambient(0.4f, 0.4f, 0.4f, 1.0f);
    lighting->set_global_ambient(global_ambient);

    // Light 0 - point light source in back right corner
    auto light_0 = std::make_shared<cg::LightNode>(0);
    light_0->set_diffuse(cg::Color4(0.5f, 0.5f, 0.5f, 1.0f));
    light_0->set_specular(cg::Color4(0.5f, 0.5f, 0.5f, 1.0f));
    light_0->set_position(cg::HPoint3(75.0f, 75.0f, 30.f, 1.0f));
    light_0->enable();

    // Light1 - directional light from the ceiling
    auto light_1 = std::make_shared<cg::LightNode>(1);
    light_1->set_diffuse(cg::Color4(0.7f, 0.7f, 0.7f, 1.0f));
    light_1->set_specular(cg::Color4(0.7f, 0.7f, 0.7f, 1.0f));
    light_1->set_position(cg::HPoint3(0.0f, 0.0f, 1.0f, 0.0f));
    light_1->enable();

    // Spotlight - reddish spotlight - we will place at the camera location
    // shining along -VPN
    Spotlight = std::make_shared<cg::LightNode>(2);
    Spotlight->set_diffuse(cg::Color4(0.5f, 0.1f, 0.1f, 1.0f));
    Spotlight->set_specular(cg::Color4(0.5f, 0.1f, 0.1f, 1.0f));
    cg::Point3 pos = g_camera->get_position();
    Spotlight->set_position(cg::HPoint3(pos.x, pos.y, pos.z, 1.0f));
    cg::Vector3 dir = g_camera->get_view_plane_normal() * -1.0f;
    Spotlight->set_spotlight(dir, 32.0f, 30.0f);
    Spotlight->enable();

    // Lights are children of the camera node
    g_camera->add_child(light_0);
    light_0->add_child(light_1);
    light_1->add_child(Spotlight);
}

/**
 * Construct the scene
 */
void construct_scene()
{
    // Shader node
    auto shader = std::make_shared<cg::LightingShaderNode>();
    if(!shader->create("Module9/pixel_lighting.vert", "Module9/pixel_lighting.frag") ||
       !shader->get_locations())
    // if(!shader->create_from_source(pixel_lighting_vert, pixel_lighting_frag) ||
    //    !shader->get_locations())
    {
        exit(-1);
    }

    // Get the position and normal locations to use when constructing VAOs
    int32_t position_loc = shader->get_position_loc();
    int32_t normal_loc = shader->get_normal_loc();

    // Add the camera to the scene
    // Initialize the view and set a perspective projection
    g_camera = std::make_shared<cg::CameraNode>();
    g_camera->set_position(cg::Point3(0.0f, -100.0f, 20.0f));
    g_camera->set_look_at_pt(cg::Point3(0.0f, 0.0f, 20.0f));
    g_camera->set_view_up(cg::Vector3(0.0f, 0.0f, 1.0f));
    g_camera->set_perspective(50.0f, 1.0f, 1.0f, 300.0f);

    // Construct fixed scene lighting
    construct_lighting(shader);

    // Construct subdivided square - subdivided 10x in both x and y
    auto unit_square = std::make_shared<cg::UnitSquareSurface>(2, position_loc, normal_loc);

    // Construct a unit cylinder surface
    auto cylinder = std::make_shared<cg::ConicSurface>(0.5f, 0.5f, 18, 4, position_loc, normal_loc);

    // Construct a unit cone
    auto cone = std::make_shared<cg::ConicSurface>(0.5f, 0.0f, 18, 4, position_loc, normal_loc);

    // Construct the room as a child of the root node
    auto room = construct_room(unit_square);

    // Construct a unit box
    auto unit_box = construct_unit_box(unit_square);

    // Construct the table
    auto table = construct_table(unit_box, cylinder);

    // Wood material for table
    auto wood = std::make_shared<cg::PresentationNode>(cg::Color4(0.275f, 0.225f, 0.075f),
                                                       cg::Color4(0.55f, 0.45f, 0.15f),
                                                       cg::Color4(0.3f, 0.3f, 0.3f),
                                                       cg::Color4(0.0f, 0.0f, 0.0f),
                                                       64.0f);

    // Position the table in the room
    auto table_transform = std::make_shared<cg::TransformNode>();
    table_transform->translate(-50.0f, 50.0f, 0.0f);
    table_transform->rotate_z(30.0f);

    // Teapot
    auto teapot = std::make_shared<cg::MeshTeapot>(4, position_loc, normal_loc);

    // Silver material (for the teapot)
    auto teapot_material =
        std::make_shared<cg::PresentationNode>(cg::Color4(0.19225f, 0.19225f, 0.19225f),
                                               cg::Color4(0.50754f, 0.50754f, 0.50754f),
                                               cg::Color4(0.508273f, 0.508273f, 0.508273f),
                                               cg::Color4(0.0f, 0.0f, 0.0f),
                                               51.2f);

    // Teapot transform. It is tough to place this - if you make too small then
    // if you look from above the table intersects the bottom, but if you move
    // higher when you look from outside it looks like the teapot is above the
    // table. This is because we don't know the exact dimensions of the teapot.
    auto teapot_transform = std::make_shared<cg::TransformNode>();
    teapot_transform->translate(0.0f, 0.0f, 26.0f);
    teapot_transform->scale(2.5f, 2.5f, 2.5f);

    // Add a box position transform (used as base transform for the box and
    // the cone on top of the box
    auto box_position_transform = std::make_shared<cg::TransformNode>();
    box_position_transform->translate(80.0f, 80.0f, 7.5f);

    // Add a material and transform for box in the back right corner
    auto box_material = std::make_shared<cg::PresentationNode>(cg::Color4(0.25f, 0.125f, 0.125f),
                                                               cg::Color4(0.5f, 0.25f, 0.25f),
                                                               cg::Color4(0.25f, 0.25f, 0.25f),
                                                               cg::Color4(0.0f, 0.0f, 0.0f),
                                                               32.0f);
    auto box_transform = std::make_shared<cg::TransformNode>();
    box_transform->rotate_z(45.0f);
    box_transform->scale(20.0f, 20.0f, 15.0f);

    // Position a golden cone on top of the box
    auto cone_material =
        std::make_shared<cg::PresentationNode>(cg::Color4(0.25f, 0.2f, 0.05f),
                                               cg::Color4(0.75164f, 0.60648f, 0.22648f),
                                               cg::Color4(0.75f, 0.75f, 0.75f),
                                               cg::Color4(0.0f, 0.0f, 0.0f),
                                               96.0f);
    auto cone_transform = std::make_shared<cg::TransformNode>();
    cone_transform->translate(0.0f, 0.0f, 15.0f);
    cone_transform->scale(8.0f, 8.0f, 15.0f);

    // Construct a vase
    auto vase = construct_vase(position_loc, normal_loc);

    // Sphere
    auto shiny_sphere = construct_shiny_sphere(position_loc, normal_loc);

    // Torus
    auto shiny_torus = construct_shiny_torus(position_loc, normal_loc);

    // Construct the scene layout
    g_scene_root = std::make_shared<cg::SceneNode>();
    g_scene_root->add_child(shader);
    shader->add_child(g_camera);

    // Construct a base node for the rest of the scene, it will be a child
    // of the last light node (so entire scene is under influence of all
    // lights)
    auto myscene = std::make_shared<cg::SceneNode>();
    Spotlight->add_child(myscene);

    // Add the room (walls, floor, ceiling)
    myscene->add_child(room);

    // Add the table
    myscene->add_child(wood);
    wood->add_child(table_transform);
    table_transform->add_child(table);

    // Add teapot as a child of the table transform.
    add_sub_tree(table_transform, teapot_material, teapot_transform, teapot);

    // Add box in the back right corner with the cone on top
    myscene->add_child(box_position_transform);
    add_sub_tree(box_position_transform, box_material, box_transform, unit_box);
    add_sub_tree(box_position_transform, cone_material, cone_transform, cone);

    // Add the vase, sphere, and torus
    myscene->add_child(vase);
    myscene->add_child(shiny_sphere);
    myscene->add_child(shiny_torus);
}

/**
 * Main
 */
int main(int argc, char **argv)
{
    cg::set_root_paths(argv[0]);

    // Print the keyboard commands
    std::cout << "i - Reset to initial view\n";
    std::cout << "R - Roll    5 degrees clockwise   r - Counter-clockwise\n";
    std::cout << "P - Pitch   5 degrees clockwise   p - Counter-clockwise\n";
    std::cout << "H - Heading 5 degrees clockwise   h - Counter-clockwise\n";
    std::cout << "X - Slide camera right            x - Slide camera left\n";
    std::cout << "Y - Slide camera up               y - Slide camera down\n";
    std::cout << "F - Move camera forward           f - Move camera backwards\n";
    std::cout << "V - Faster mouse movement         v - Slower mouse movement\n";
    std::cout << "ESC - Exit Program\n";

    // Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Initialize display mode and window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_PropertiesID props = SDL_CreateProperties();
    if(props == 0)
    {
        std::cout << "Error creating SDL Window Properties: " << SDL_GetError() << '\n';
        exit(1);
    }

    SDL_SetStringProperty(
        props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Simple 3-D Scene by Brian Russin");
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 800);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 600);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 100);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 100);

    g_sdl_window = SDL_CreateWindowWithProperties(props);
    if(g_sdl_window == nullptr)
    {
        std::cout << "Error initializing SDL Window" << SDL_GetError() << '\n';
        exit(1);
    }

    // Initialize OpenGL
    g_gl_context = SDL_GL_CreateContext(g_sdl_window);

    std::cout << "OpenGL  " << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

#if BUILD_WINDOWS
    int32_t glew_init_result = glewInit();
    if(GLEW_OK != glew_init_result)
    {
        std::cout << "GLEW Error: " << glewGetErrorString(glew_init_result) << '\n';
        exit(EXIT_FAILURE);
    }
#endif

    // Set the clear color to black. Any part of the window outside the
    // viewport should appear black
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Construct scene.
    construct_scene();

    // Enable multi-sample anti-aliasing
    glEnable(GL_MULTISAMPLE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable back face polygon removal
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    reshape(800, 600);

    update_view(g_mouse_x, g_mouse_y, g_forward);

    // Main loop
    while(handle_events())
    {
        if(g_animate) update_view(g_mouse_x, g_mouse_y, g_forward);
        display();
        sleep(DRAW_INTERVAL_MILLIS);
    }

    // Destroy OpenGL Context, SDL Window and SDL
    SDL_GL_DestroyContext(g_gl_context);
    SDL_DestroyWindow(g_sdl_window);
    SDL_Quit();

    return 0;
}
