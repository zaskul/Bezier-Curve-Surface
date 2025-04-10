#include <math.h>
#include <vector>
#include <string>
#include <regex>
#include "../header_files/Utils.h"

namespace Utils {
    float calc_bernstein(float vw, int ij) {
        float res = -1.0f;
        switch (ij)
        {
        case 0:
            res = (float)(pow(1 - vw, 3) * pow(vw, 0));
            break;
        case 1:
            res = (float)(3 * pow(1 - vw, 2) * pow(vw, 1));
            break;
        case 2:
            res = (float)(3 * pow(1 - vw, 1) * pow(vw, 2));
            break;
        case 3:
            res = (float)(pow(1 - vw, 0) * pow(vw, 3));
            break;
        default:
            res = -1.0f;
            break;
        }
        return res;
    }

    std::vector<std::string> split_string(std::string str, std::string pattern) {
        std::regex regex_pattern(pattern);
        std::sregex_token_iterator iter(str.begin(), str.end(), regex_pattern, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> tokens(iter, end);
        return tokens;
    }
}