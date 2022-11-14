//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_SDL2OPENGLWINDOW_H
#define DREAM_SDL2OPENGLWINDOW_H

#include <SDL2/SDL.h>
#include "dream/window/SDL2Window.h"

namespace Dream {
    class SDL2OpenGLWindow : public SDL2Window {
    public:
        SDL2OpenGLWindow();
        void swapBuffers() override;
        SDL_GLContext getSDL2GLContext();
    protected:
        void pollEditorEvents(SDL_Event &Event) override;
    private:
        SDL_GLContext glContext;
    };
}

#endif //DREAM_SDL2OPENGLWINDOW_H
