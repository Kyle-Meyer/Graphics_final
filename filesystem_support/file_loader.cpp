

#include "filesystem_support/file_loader.hpp"

#include <fstream>

namespace cg
{

FileContents::FileContents() : size{0}, data{nullptr} {}

void FileContents::init(uint32_t size_in)
{
    size = size_in;
    data = new char[size + 1];
}

void FileContents::destroy()
{
    if(data != nullptr) delete[] data;
    data = nullptr;
    size = 0;
}

bool load_file_contents(const std::string &path, FileContents &file_contents)
{
    std::ifstream ifs;
    ifs.open(path);
    if(!ifs.is_open()) return false;

    ifs.seekg(0, std::ios::end);

    uint32_t size = static_cast<uint32_t>(ifs.tellg());
    file_contents.init(size);

    ifs.seekg(0, std::ios::beg);
    ifs.read(file_contents.data, size);
    file_contents.data[size] = (char)0x0;
    ifs.close();
    return true;
}

} // namespace cg
