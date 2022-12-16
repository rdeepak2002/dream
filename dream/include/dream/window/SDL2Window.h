//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_SDL2WINDOW_H
#define DREAM_SDL2WINDOW_H

#include "dream/window/Window.h"
#include <SDL2/SDL.h>

namespace Dream {
    class SDL2Window : public Window {
    public:
        ~SDL2Window();
        void update(float dt) override;
        void pollEvents() override;
        void swapBuffers() override;
        std::pair<int, int> getWindowDimensions() override;
        SDL_Window *getSDL2Window();
    private:
        bool firstLoad;
    protected:
        SDL2Window(Uint32 flags);
        virtual void pollEditorEvents(SDL_Event &Event);
        #ifndef EMSCRIPTEN
        SDL_Window *launchWindow;
        #endif
        SDL_Window *sdlWindow;
        int windowWidth, windowHeight;
    };
}

#endif //DREAM_SDL2WINDOW_H
