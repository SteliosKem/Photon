#include "Common.h"
#include <fstream>
#include <iostream>

namespace Photon {
    string read_file(const Filepath& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        std::cout << "Opening: " << path << '\n';
        if(!file.is_open())
            throw std::runtime_error("Failed to open file '" + path.string() + "'");
        
        size_t filesize = file.tellg();
        vector<char> buffer(filesize);

        file.seekg(0);
        file.read(buffer.data(), filesize);

        file.close();
        return string(buffer.begin(), buffer.end());
    }
}