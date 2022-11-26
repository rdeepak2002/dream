//
// Created by Deepak Ramalingam on 11/26/22.
//

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

namespace Dream {
    class Logger {
    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }
        static void Debug(const std::string& text);
        static void Error(const std::string& text);
        static void Warn(const std::string& text);
    private:
        Logger() = default;
    public:
        Logger(Logger const&) = delete;
        void operator=(Logger const&) = delete;
    };
}

#endif //EDITOR_LOGGER_H
