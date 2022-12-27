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

#ifndef DREAM_LOGCOLLECTOR_H
#define DREAM_LOGCOLLECTOR_H

#include "dream/util/LoggerListener.h"
#include <vector>

#define MAX_LOGS 100

namespace Dream {
    enum LogType { debug, warn, error, info, fatal };
    struct Log {
        LogType type;
        std::string text;
    };
    class LogCollector : public LoggerListener {
    private:
        std::vector<Log> logs;
    public:
        LogCollector();
        void debugLogPublished(std::string text) override;
        void warnLogPublished(std::string text) override;
        void errorLogPublished(std::string text) override;
        void infoLogPublished(std::string text) override;
        void fatalLogPublished(std::string text) override;
        std::vector<Log> getLogs();
        void clearLogs();
    };
}

#endif //DREAM_LOGCOLLECTOR_H
