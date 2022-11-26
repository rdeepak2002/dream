//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/util/Logger.h"

namespace Dream {
    void Logger::Debug(const std::string& text) {
        std::string formattedText = "DEBUG: " + text;
#if defined(EMSCRIPTEN)
        printf("%s\n", formattedText.c_str());
#else
        printf("%s%s%s\n", KBLU, formattedText.c_str(), KNRM);
#endif
    }

    void Logger::Error(const std::string& text) {
        std::string formattedText = "ERROR: " + text;
#if defined(EMSCRIPTEN)
        printf("%s\n", formattedText.c_str());
#else
        printf("%s%s%s\n", KRED, formattedText.c_str(), KNRM);
#endif
    }

    void Logger::Warn(const std::string& text) {
        std::string formattedText = "WARNING: " + text;
#if defined(EMSCRIPTEN)
        printf("%s\n", formattedText.c_str());
#else
        printf("%s%s%s\n", KYEL, formattedText.c_str(), KNRM);
#endif
    }
}
