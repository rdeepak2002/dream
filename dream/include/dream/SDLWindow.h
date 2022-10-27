//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_SDLWINDOW_H
#define DREAM_SDLWINDOW_H

#include <SDL2/SDL.h>

class SDLWindow {
public:
    SDLWindow();
    ~SDLWindow();
    void pollEvents();
    void swapBuffers();
    bool shouldClose();
    std::pair<int, int> getWindowDimensions();
private:
    SDL_Window *Window;
    bool shouldCloseFlag;
    int windowWidth, windowHeight;
};


#endif //DREAM_SDLWINDOW_H
