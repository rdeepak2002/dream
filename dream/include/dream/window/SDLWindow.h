//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_SDLWINDOW_H
#define DREAM_SDLWINDOW_H

#include <SDL2/SDL.h>

namespace Dream {
    class SDLWindow {
    public:
        SDLWindow();
        ~SDLWindow();
        void pollEvents();
        void swapBuffers();
        bool shouldClose();
        std::pair<int, int> getWindowDimensions();
        SDL_Window *getSDL2Window();
        SDL_GLContext getSDL2GLContext();
    private:
        SDL_Window *Window;
        bool shouldCloseFlag;
        int windowWidth, windowHeight;
        SDL_GLContext glContext;
    };
}

#endif //DREAM_SDLWINDOW_H
