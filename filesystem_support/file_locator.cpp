

#include "filesystem_support/file_locator.hpp"
#include "filesystem_support/preprocessor.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>

namespace cg
{

namespace
{
std::string executable_path;
std::string source_path;
} // namespace

std::string correct_path_separators(const std::string &path)
{
#ifdef _WIN32
    char incorrect_sep = '/';
    char correct_sep = '\\';
#else
    char incorrect_sep = '\\';
    char correct_sep = '/';
#endif

    std::string correct_str(path);
    std::replace(correct_str.begin(), correct_str.end(), incorrect_sep, correct_sep);
    return correct_str;
}

void set_root_paths(const char *exec_name)
{
    source_path = STD_STRING(SRC_ROOT);

    // find last '/'
    size_t max = 0;
    size_t idx = 0;
    while(exec_name[idx] != '\0' && idx < 1000)
    {
        if(exec_name[idx] == '/') max = idx;
        ++idx;
    }
    char *exec_path = (char *)malloc(idx);
    memcpy(exec_path, exec_name, idx);
    exec_path[max] = '\0';

    executable_path = std::string(exec_path) + std::string("/");
    free(exec_path);
}

FileInfo locate_path_for_filename_with_prefix(const std::string &prefix,
                                              const std::string &filename,
                                              uint16_t           num_directories)
{
    FileInfo result;
    result.found = false;

    std::string   rel_path = filename;
    std::ifstream ifs;
    for(int i = 0; i < num_directories && !result.found; ++i)
    {
        std::string t_str = prefix + rel_path;
        std::string abs_path = correct_path_separators(t_str);
        ifs.open(abs_path);
        result.found = ifs.is_open();
        if(result.found) result.file_path = abs_path;
        rel_path.insert(0, "../");
    }

    return result;
}

FileInfo locate_path_for_filename(const std::string &filename, uint16_t num_directories)
{
    FileInfo result;
    result.found = false;

    std::string rel_path = filename;

    // Look for the file locally first
    result = locate_path_for_filename_with_prefix("", filename, num_directories);

    // if not found, try looking at the executable path
    if(!result.found)
        result = locate_path_for_filename_with_prefix(executable_path, filename, num_directories);

    // if still not found, try looking at the source path
    if(!result.found)
        result = locate_path_for_filename_with_prefix(source_path, filename, num_directories);

    if(!result.found) return result;

    std::ifstream ifs;
    ifs.open(result.file_path);
    ifs.seekg(0, std::ios::end);
    result.file_size = static_cast<uint32_t>(ifs.tellg());
    ifs.close();

    return result;
}

} // namespace cg
