#include "functions.h"
#include <cstring>

namespace nSXFFile {
    void InformationFlagsPrint(uint32_t p_ul_info) {
        char binary[sizeof(p_ul_info) * 8];
        std::memset(binary, 0, sizeof(p_ul_info) * 8);
        int i = 0;
        while (p_ul_info != 0) {
            binary[i] = p_ul_info % 2;
            p_ul_info /= 2;
            ++i;
        }
        for (i = sizeof(p_ul_info) * 8 - 1; i >= 0; --i) {
            if ((i + 1) % 8 == 0) {
                std::cout << " ";
            }
            std::cout << (int)binary[i];
        }
        std::cout << std::endl;
    }
}
