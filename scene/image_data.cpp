#include "scene/image_data.hpp"

#include "filesystem_support/file_locator.hpp"

// https://github.com/nothings/stb
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#endif

#include <cstring>
#include <iostream>

namespace cg
{

void flip_image_data(ImageData &im_data)
{
    const size_t   bytes_per_row = im_data.w * im_data.channels;
    unsigned char *tmp_row_buffer = new unsigned char[bytes_per_row];
    unsigned char *bottom_row_ptr = im_data.data;
    unsigned char *top_row_ptr = im_data.data + bytes_per_row * (im_data.h - 1);

    for(size_t i = 0; i < im_data.h / 2; ++i)
    {
        // store the top row into the temporary buffer
        std::memcpy(tmp_row_buffer, top_row_ptr, bytes_per_row);

        // put the contents of the bottom row into the top row
        std::memcpy(top_row_ptr, bottom_row_ptr, bytes_per_row);

        // put the contents of the temporary buffer into the bottom row
        std::memcpy(bottom_row_ptr, tmp_row_buffer, bytes_per_row);

        // increment the bottom row pointer
        bottom_row_ptr += bytes_per_row;
        // decrement the top row pointer
        top_row_ptr -= bytes_per_row;
    }

    delete[] tmp_row_buffer;
}

void load_image_data(ImageData &im_data, const std::string &filename, bool include_alpha)
{
    auto file_info = locate_path_for_filename(filename, 5);

    if(!file_info.found)
    {
        // If file not found, try a directory called textures, going up a few directories
        std::string tex_fname = std::string("textures/") + filename;
        file_info = locate_path_for_filename(tex_fname, 5);
    }

    if(!file_info.found)
    {
        std::cout << "Error getting finding file " << filename << '\n';
        return;
    }
    stbi_set_flip_vertically_on_load(true);
    im_data.data = stbi_load(file_info.file_path.c_str(),
                             &im_data.w,
                             &im_data.h,
                             &im_data.channels,
                             include_alpha ? STBI_rgb_alpha : STBI_rgb);

    // We are explicityly setting the loaded image to RGB or RGBA (using STBI_rgb or STBI_rgb_alpha)
    im_data.channels = include_alpha ? 4 : 3;

    if(im_data.data == nullptr)
    {
        std::cout << "Error getting image data for " << filename << '\n';
        return;
    }

    flip_image_data(im_data);
}

void free_image_data(ImageData &im_data)
{
    if(im_data.data == nullptr) return;

    // Free the image data
    stbi_image_free(im_data.data);
    im_data.data = nullptr;
}

} // namespace cg
