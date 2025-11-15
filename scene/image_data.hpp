///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  Brian Russin
//	File:	image_data.hpp
//	Purpose: Loads an image from file.
//============================================================================

#ifndef __SCENE_IMAGE_DATA_HPP__
#define __SCENE_IMAGE_DATA_HPP__

#include <string>

namespace cg
{

struct ImageData
{
    int32_t        w = 0;
    int32_t        h = 0;
    int32_t        channels = 0;
    unsigned char *data = nullptr;
};

void flip_image_data(ImageData &im_data);

void load_image_data(ImageData &im_data, const std::string &filename, bool include_alpha = true);

void free_image_data(ImageData &im_data);

} // namespace cg

#endif
