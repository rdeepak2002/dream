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

#include "dream/editor/LogCollector.h"
#include "dream/util/Logger.h"

namespace Dream {
    LogCollector::LogCollector() {
        Logger::registerLoggerListener(this);
    }

    void LogCollector::debugLogPublished(std::string text) {
        logs.push_back({
            .type=LogType::debug,
            .text=text
        });

        while (logs.size() > MAX_LOGS) {
            logs.erase(logs.begin());
        }
    }

    void LogCollector::warnLogPublished(std::string text) {
        logs.push_back({
            .type=LogType::warn,
            .text=text
        });

        if (logs.size() > MAX_LOGS) {
            logs.erase(logs.begin());
        }
    }

    void LogCollector::errorLogPublished(std::string text) {
        logs.push_back({
            .type=LogType::error,
            .text=text
        });

        while (logs.size() > MAX_LOGS) {
            logs.erase(logs.begin());
        }
    }

    void LogCollector::infoLogPublished(std::string text) {
        logs.push_back({
            .type=LogType::info,
            .text=text
        });

        while (logs.size() > MAX_LOGS) {
            logs.erase(logs.begin());
        }
    }

    void LogCollector::fatalLogPublished(std::string text) {
        logs.push_back({
            .type=LogType::fatal,
            .text=text
        });

        while (logs.size() > MAX_LOGS) {
            logs.erase(logs.begin());
        }
    }

    std::vector<Log> LogCollector::getLogs() {
        return logs;
    }

    void LogCollector::clearLogs() {
        logs.clear();
    }
}
