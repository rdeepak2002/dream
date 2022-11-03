//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_SDL2WINDOW_H
#define DREAM_SDL2WINDOW_H

#include <SDL2/SDL.h>

namespace Dream {
    class SDL2Window {
    public:
        ~SDL2Window();
        virtual void pollEvents();
        virtual void swapBuffers();
        [[nodiscard]] bool shouldClose() const;
        std::pair<int, int> getWindowDimensions();
        SDL_Window *getSDL2Window();
    protected:
        SDL2Window(Uint32 flags);
        virtual void pollEditorEvents(SDL_Event &Event);
        SDL_Window *Window;
        bool shouldCloseFlag;
        int windowWidth, windowHeight;
    };
}

#endif //DREAM_SDL2WINDOW_H
