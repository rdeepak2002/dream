//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/util/Logger.h"

namespace Dream {
    void Logger::debug(const std::string& text) {
        printf("%s%s%s%s\n", KCYN, "[D] ", KNRM, text.c_str());
        if (getInstance().loggerListener) {
            getInstance().loggerListener->debugLogPublished(text);
        }
    }

    void Logger::error(const std::string& text) {
        printf("%s%s%s%s\n", KRED, "[E] ", KNRM, text.c_str());
        if (getInstance().loggerListener) {
            getInstance().loggerListener->errorLogPublished(text);
        }
    }

    void Logger::warn(const std::string& text) {
        printf("%s%s%s%s\n", KYEL, "[W] ", KNRM, text.c_str());
        if (getInstance().loggerListener) {
            getInstance().loggerListener->warnLogPublished(text);
        }
    }

    void Logger::info(const std::string& text) {
        printf("%s%s%s%s\n", KGRN, "[I] ", KNRM, text.c_str());
        if (getInstance().loggerListener) {
            getInstance().loggerListener->infoLogPublished(text);
        }
    }

    void Logger::fatal(const std::string& text, bool endProgram) {
        printf("%s%s%s%s\n", KMAG, "[F] ", KNRM, text.c_str());
        if (getInstance().loggerListener) {
            getInstance().loggerListener->fatalLogPublished(text);
        }
        if (endProgram) {
            exit(EXIT_FAILURE);
        }
    }

    void Logger::registerLoggerListener(LoggerListener *loggerListener) {
        getInstance().loggerListener = loggerListener;
    }
}
