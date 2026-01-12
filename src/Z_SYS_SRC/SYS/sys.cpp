#include "syst.hpp"

std::string trim_file_path(const std::string& full_path) {
    size_t src_pos = full_path.find("src/");
    return (src_pos != std::string::npos) ? full_path.substr(src_pos) : full_path;
}
