///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  Brian Russin
//	File:    file_locator.hpp
//	Purpose: Locates file on the filesystem.
//============================================================================

#ifndef __FILESYSTEM_SUPPORT_FILE_LOCATOR_HPP__
#define __FILESYSTEM_SUPPORT_FILE_LOCATOR_HPP__

#include <cstdint>
#include <string>

namespace cg
{

struct FileInfo
{
    bool        found = false;
    std::string file_path;
    uint64_t    file_size = 0;
};

std::string correct_path_separators(const std::string &path);

void set_root_paths(const char *exec_name);

FileInfo locate_path_for_filename_with_prefix(const std::string &prefix,
                                              const std::string &filename,
                                              uint16_t           num_directories = 5);

FileInfo locate_path_for_filename(const std::string &filename, uint16_t num_directories = 5);

} // namespace cg

#endif
