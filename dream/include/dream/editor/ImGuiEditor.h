//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_IMGUIEDITOR_H
#define DREAM_IMGUIEDITOR_H

#include "dream/window/SDL2OpenGLWindow.h"

namespace Dream {
    class ImGuiEditor {
    public:
        void update(Dream::SDL2OpenGLWindow *window, unsigned int frameBufferTexture);
        void style();
        std::pair<int, int> getRendererViewportDimensions();
    protected:
        explicit ImGuiEditor(Dream::SDL2OpenGLWindow *window);
        virtual void newFrame(Dream::SDL2OpenGLWindow *window);
        virtual void renderDrawData();
        int rendererViewportWidth, rendererViewportHeight;
    };
}


#endif //DREAM_IMGUIEDITOR_H
