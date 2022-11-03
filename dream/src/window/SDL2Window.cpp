//
// Created by Deepak Ramalingam on 11/3/22.
//

#include "dream/window/SDL2Window.h"

#include <iostream>

namespace Dream {
    SDL2Window::SDL2Window(Uint32 flags) {
        this->windowWidth = 1024;
        this->windowHeight = 768;
        Uint32 WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | flags;
        this->Window = SDL_CreateWindow("Dream", 0, 0, this->windowWidth, this->windowHeight, WindowFlags);
        this->shouldCloseFlag = false;
    }

    SDL2Window::~SDL2Window() {
        SDL_DestroyWindow(Window);
    }

    void SDL2Window::pollEvents() {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            this->pollEditorEvents(Event);
            if (Event.type == SDL_KEYDOWN)
            {
                switch (Event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        this->shouldCloseFlag = true;
                        break;
                    default:
                        break;
                }
            }
            else if (Event.type == SDL_WINDOWEVENT) {
                switch (Event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        this->windowWidth = Event.window.data1;
                        this->windowHeight = Event.window.data2;
                        break;
                }
            }
            else if (Event.type == SDL_QUIT)
            {
                this->shouldCloseFlag = true;
            }
        }
    }

    void SDL2Window::swapBuffers() {

    }

    bool SDL2Window::shouldClose() const {
        return this->shouldCloseFlag;
    }

    std::pair<int, int> SDL2Window::getWindowDimensions() {
        return std::make_pair(this->windowWidth, this->windowHeight);
    }

    SDL_Window *SDL2Window::getSDL2Window() {
        return this->Window;
    }

    void SDL2Window::pollEditorEvents(SDL_Event &Event) {

    }
}
