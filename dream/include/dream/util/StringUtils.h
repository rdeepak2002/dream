//
// Created by Deepak Ramalingam on 12/16/22.
//

#ifndef DREAM_STRINGUTILS_H
#define DREAM_STRINGUTILS_H

#include <iostream>

namespace Dream {
    class StringUtils {
    public:
        static std::string getFilePathRelativeToProjectFolder(std::string path);
        static std::string shorten(std::string str, bool ellipsis = true, int maxLength = 5);
    };
}

#endif //DREAM_STRINGUTILS_H
