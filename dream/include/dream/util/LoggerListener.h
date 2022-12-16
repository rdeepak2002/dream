//
// Created by Deepak Ramalingam on 12/10/22.
//

#ifndef DREAM_LOGGERLISTENER_H
#define DREAM_LOGGERLISTENER_H

#include <iostream>

namespace Dream {
    class LoggerListener {
    public:
        virtual void debugLogPublished(std::string text) = 0;
        virtual void warnLogPublished(std::string text) = 0;
        virtual void errorLogPublished(std::string text) = 0;
        virtual void infoLogPublished(std::string text) = 0;
        virtual void fatalLogPublished(std::string text) = 0;
    };
}

#endif //DREAM_LOGGERLISTENER_H
