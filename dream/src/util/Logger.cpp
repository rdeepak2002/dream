//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/util/Logger.h"

namespace Dream {
    void Logger::debug(const std::string& text) {
#if defined(EMSCRIPTEN)
        printf("%s%s%s\n", KBLU, text.c_str(), KNRM);
#else
        printf("%s%s%s\n", KBLU, text.c_str(), KNRM);
#endif
    }

    void Logger::error(const std::string& text) {
#if defined(EMSCRIPTEN)
        printf("%s%s%s\n", KRED, text.c_str(), KNRM);
#else
        printf("%s%s%s\n", KRED, text.c_str(), KNRM);
#endif
    }

    void Logger::warn(const std::string& text) {
#if defined(EMSCRIPTEN)
        printf("%s%s%s\n", KYEL, text.c_str(), KNRM);
#else
        printf("%s%s%s\n", KYEL, text.c_str(), KNRM);
#endif
    }
}
