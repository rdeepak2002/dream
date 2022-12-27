/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

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
        SDL_Renderer* launchWindowRenderer;
        SDL_Texture *launchWindowImage;
    };
}

#endif //DREAM_SDL2WINDOW_H
