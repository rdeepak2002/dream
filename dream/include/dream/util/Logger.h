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

#ifndef EDITOR_LOGGER_H
#define EDITOR_LOGGER_H

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#include <iostream>
#include "dream/util/LoggerListener.h"

namespace Dream {
    class Logger {
    public:
        static Logger &getInstance() {
            static Logger instance;
            return instance;
        }

        static void debug(const std::string &text);

        static void error(const std::string &text);

        static void warn(const std::string &text);

        static void info(const std::string &text);

        static void fatal(const std::string &text, bool endProgram = true);

        static void registerLoggerListener(LoggerListener *loggerListener);

    private:
        Logger() = default;

        LoggerListener *loggerListener;
    public:
        Logger(Logger const &) = delete;

        void operator=(Logger const &) = delete;
    };
}

#endif //EDITOR_LOGGER_H
