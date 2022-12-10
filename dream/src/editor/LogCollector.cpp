//
// Created by Deepak Ramalingam on 12/10/22.
//

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
}
