//
// Created by Deepak Ramalingam on 12/10/22.
//

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
    };
}

#endif //DREAM_LOGCOLLECTOR_H
