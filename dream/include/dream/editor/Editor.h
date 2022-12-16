//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_EDITOR_H
#define DREAM_EDITOR_H

#include "dream/window/Window.h"
#include "LogCollector.h"

namespace Dream {
    class Editor {
    public:
        virtual void update(Dream::Window *window, unsigned int frameBufferTexture);
        virtual std::pair<int, int> getRendererViewportDimensions();
        void setLogCollector(LogCollector* logCollector);
    protected:
        explicit Editor(Dream::Window *window);
        virtual void newFrame(Dream::Window *window);
        virtual void renderDrawData();
        LogCollector* logCollector;
    };
}

#endif //DREAM_EDITOR_H
