//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_IMGUIEDITOR_H
#define DREAM_IMGUIEDITOR_H

#include "dream/window/SDLWindow.h"

namespace Dream {
    class ImGuiEditor {
    public:
        void update(Dream::SDLWindow *window, unsigned int frameBufferTexture);
        void style();
        std::pair<int, int> getRendererViewportDimensions();
    protected:
        explicit ImGuiEditor(Dream::SDLWindow *window);
        virtual void newFrame(Dream::SDLWindow *window);
        virtual void renderDrawData();
        int rendererViewportWidth, rendererViewportHeight;
    };
}


#endif //DREAM_IMGUIEDITOR_H
