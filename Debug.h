#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <fstream>
namespace DEBUG_TOOLS
{
    static void debug_log(std::string msg1, std::string msg2) {
        fs::path path{ "C:\\TMP\\debug.log" };
        fs::create_directories(path.parent_path());

        std::ofstream ofs(path, std::ios_base::app);
        ofs << msg1 << "    " << msg2 << std::endl;
        ofs.close();
    };
}