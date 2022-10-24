//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_SDLWINDOW_H
#define DREAM_SDLWINDOW_H

#include <SDL2/SDL.h>

class SDLWindow {
public:
    SDLWindow(int windowWidth, int windowHeight);
    void pollEvents();
    void swapBuffers();
    bool shouldClose();
private:
    SDL_Window *Window;
    bool shouldCloseFlag;
};


#endif //DREAM_SDLWINDOW_H
