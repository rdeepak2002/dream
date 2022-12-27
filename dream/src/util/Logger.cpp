/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

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
        printf("%s%s%s%s\n", KRED, "[F] ", KNRM, text.c_str());
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
