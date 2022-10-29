//
// Created by Deepak Ramalingam on 10/27/22.
//

#ifndef DREAM_EDITOR_H
#define DREAM_EDITOR_H

#include "dream/SDLWindow.h"

namespace Dream {
    class Editor {
    public:
        Editor(Dream::SDLWindow *window);
        void update(Dream::SDLWindow *window, unsigned int frameBufferTexture);
        void style();
        static void pollEvents(SDL_Event Event);
        std::pair<int, int> getRendererViewportDimensions();
    private:
        int rendererViewportWidth, rendererViewportHeight;
    };
}

#endif //DREAM_EDITOR_H
