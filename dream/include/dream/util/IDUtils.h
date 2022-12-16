//
// Created by Deepak Ramalingam on 11/18/22.
//

#ifndef DREAM_IDUTILS_H
#define DREAM_IDUTILS_H

#include <iostream>
#include <random>
#include <sstream>
#include "dream/util/MD5.h"

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4();
}

namespace Dream {
    class IDUtils {
    public:
        static std::string newGUID();
        static std::string newFileID(std::string uniqueIdentifier);
        static std::string getGUIDForFile(std::string filepath);
    };
}

#endif //DREAM_IDUTILS_H
