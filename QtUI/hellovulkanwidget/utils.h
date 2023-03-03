#pragma once

#include <string>
#include <iostream>
#include <cstdio>

namespace UTILS {
    inline bool check_file_exsist(const std::string& file_path) {
        FILE* fp = nullptr;
        if (fopen_s(&fp, file_path.c_str(), "r")) {
            fclose(fp);
            return true;
        }
        else {
            return false;
        }
    }
}
