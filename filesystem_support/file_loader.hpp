///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  Brian Russin
//	File:    file_loader.hpp
//	Purpose: Loads file contents from the filesystem.
//============================================================================

#ifndef __FILESYSTEM_SUPPORT_FILE_LOADER_HPP__
#define __FILESYSTEM_SUPPORT_FILE_LOADER_HPP__

#include <cstdint>
#include <string>

namespace cg
{

struct FileContents
{
    uint32_t size;
    char    *data;

    FileContents();

    void init(uint32_t size_in);
    void destroy();
};

bool load_file_contents(const std::string &path, FileContents &file_contents);

} // namespace cg

#endif
