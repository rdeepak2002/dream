//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/util/Logger.h"

namespace Dream {
    void Logger::debug(const std::string& text) {
        std::string formattedText = "DEBUG: " + text;
#if defined(EMSCRIPTEN)
        printf("%s\n", formattedText.c_str());
#else
        printf("%s%s%s\n", KBLU, formattedText.c_str(), KNRM);
#endif
    }

    void Logger::error(const std::string& text) {
        std::string formattedText = "ERROR: " + text;
#if defined(EMSCRIPTEN)
        printf("%s\n", formattedText.c_str());
#else
        printf("%s%s%s\n", KRED, formattedText.c_str(), KNRM);
#endif
    }

    void Logger::warn(const std::string& text) {
        std::string formattedText = "WARNING: " + text;
#if defined(EMSCRIPTEN)
        printf("%s\n", formattedText.c_str());
#else
        printf("%s%s%s\n", KYEL, formattedText.c_str(), KNRM);
#endif
    }
}
