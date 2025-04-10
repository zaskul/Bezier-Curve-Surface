#pragma once
#include <vector>
#include <string>

namespace Utils {
    float calc_bernstein(float vw, int ij);
    std::vector<std::string> split_string(std::string str, std::string pattern);
}
