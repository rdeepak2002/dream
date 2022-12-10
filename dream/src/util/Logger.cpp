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
        if (getInstance().loggerListener) {
            getInstance().loggerListener->debugLogPublished(text);
        }
    }

    void Logger::error(const std::string& text) {
#if defined(EMSCRIPTEN)
        printf("%s%s%s\n", KRED, text.c_str(), KNRM);
#else
        printf("%s%s%s\n", KRED, text.c_str(), KNRM);
#endif
        if (getInstance().loggerListener) {
            getInstance().loggerListener->warnLogPublished(text);
        }
    }

    void Logger::warn(const std::string& text) {
#if defined(EMSCRIPTEN)
        printf("%s%s%s\n", KYEL, text.c_str(), KNRM);
#else
        printf("%s%s%s\n", KYEL, text.c_str(), KNRM);
#endif
        if (getInstance().loggerListener) {
            getInstance().loggerListener->errorLogPublished(text);
        }
    }

    void Logger::registerLoggerListener(LoggerListener *loggerListener) {
        getInstance().loggerListener = loggerListener;
    }
}
