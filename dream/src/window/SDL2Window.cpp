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

#include "dream/window/SDL2Window.h"

#include <thread>
#include <chrono>
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"
#include "dream/project/Project.h"
#include "dream/Application.h"
#include <SDL2/SDL_image.h>

#ifdef BORDERLESS
static SDL_HitTestResult SDLCALL hitTest(SDL_Window *window, const SDL_Point *pt, void *data) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    const SDL_Rect dragAreas[] = {
            { 0, 0, w, 30 },
    };
    int numDragAreas = SDL_arraysize(dragAreas);
    for (int i = 0; i < numDragAreas; i++) {
        if (SDL_PointInRect(pt, &dragAreas[i])) {
            return SDL_HITTEST_DRAGGABLE;
        }
    }
    return SDL_HITTEST_NORMAL;
}
#endif

namespace Dream {
    SDL2Window::SDL2Window(Uint32 flags) : sdlWindow() {
        this->firstLoad = true;
        this->isLoading = true;
        this->windowWidth = 1600;
        this->windowHeight = 900;
        this->launchWindowRenderer = nullptr;
        this->launchWindowImage = nullptr;
        Uint32 WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | flags;
#ifdef BORDERLESS
        WindowFlags |= SDL_WINDOW_BORDERLESS;
#endif
#ifndef EMSCRIPTEN
        // window when program launches to indicate loading
        this->launchWindow = SDL_CreateWindow("Dream", 0, 0, 1248 / 2, 778 / 2,
                                              SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);
        SDL_SetWindowPosition(this->launchWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(this->launchWindow);
        SDL_RaiseWindow(this->launchWindow);
#endif
        // main window with editor
        this->sdlWindow = SDL_CreateWindow("Dream", 0, 0, this->windowWidth, this->windowHeight, WindowFlags);
        SDL_SetWindowPosition(this->sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_RaiseWindow(this->sdlWindow);
#ifndef EMSCRIPTEN
        SDL_HideWindow(this->sdlWindow);
#endif
#ifdef BORDERLESS
        SDL_SetWindowHitTest(this->Window, hitTest, nullptr);
#endif
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    SDL2Window::~SDL2Window() {
        SDL_DestroyWindow(this->sdlWindow);
#ifndef EMSCRIPTEN
        SDL_DestroyWindow(this->launchWindow);
#endif
    }

    void SDL2Window::update(float dt) {
#ifndef EMSCRIPTEN
        float diagDPI = -1;
        float horiDPI = -1;
        float vertDPI = -1;
        int dpiReturn = SDL_GetDisplayDPI (0, &diagDPI, &horiDPI, &vertDPI);
        bool highDpi = true;

        #ifdef APPLE
        highDpi = (diagDPI > 100);
        #endif

        // draw Dream logo in launch screen
        if (!launchWindowRenderer) {
            launchWindowRenderer = SDL_CreateRenderer(launchWindow, -1, SDL_RENDERER_ACCELERATED);
            std::string launchWindowImagePath = Application::getResourcesRoot().append(
                    "assets").append("logo").append("startup-logo.png");
            launchWindowImage = IMG_LoadTexture(launchWindowRenderer, launchWindowImagePath.c_str());
        }
        SDL_RenderClear(launchWindowRenderer);
        SDL_SetRenderDrawColor(launchWindowRenderer, 255, 255, 255, 0);
        SDL_Rect textureBounds = {0, 0, 1248 / (highDpi ? 1 : 2), 778 / (highDpi ? 1 : 2)};
        SDL_RenderCopy(launchWindowRenderer, launchWindowImage, nullptr, &textureBounds);
        SDL_RenderPresent(launchWindowRenderer);

        if (!isLoading && firstLoad) {
//            std::this_thread::sleep_for(std::chrono::seconds(1));
            SDL_HideWindow(this->launchWindow);
            SDL_DestroyWindow(this->launchWindow);
            SDL_DestroyTexture(launchWindowImage);
            SDL_DestroyRenderer(this->launchWindowRenderer);
            SDL_ShowWindow(this->sdlWindow);
            this->firstLoad = false;
        }
#endif
        if (Input::pointerLockActivated()) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        } else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }

    void SDL2Window::pollEvents() {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            this->pollEditorEvents(Event);
            if (Event.type == SDL_KEYDOWN) {
                bool state = true;
                switch (Event.key.keysym.sym) {
                    case SDLK_DOWN:
                        Input::setButtonDown(Key::Down, state);
                        break;
                    case SDLK_UP:
                        Input::setButtonDown(Key::Up, state);
                        break;
                    case SDLK_LEFT:
                        Input::setButtonDown(Key::Left, state);
                        break;
                    case SDLK_RIGHT:
                        Input::setButtonDown(Key::Right, state);
                        break;
                    case SDLK_SPACE:
                        Input::setButtonDown(Key::Space, state);
                        break;
                    case SDLK_LSHIFT:
                        Input::setButtonDown(Key::LeftShift, state);
                        break;
                    case SDLK_RSHIFT:
                        Input::setButtonDown(Key::RightShift, state);
                        break;
                    case SDLK_ESCAPE:
                        Input::setButtonDown(Key::Escape, state);
                        Input::activatePointerLock(false);
                        break;
                    default:
                        int keyCode = (int) Event.key.keysym.sym;
                        Input::setButtonDown(keyCode, state);
                        break;
                }
            } else if (Event.type == SDL_KEYUP) {
                bool state = false;
                switch (Event.key.keysym.sym) {
                    case SDLK_DOWN:
                        Input::setButtonDown(Key::Down, state);
                        break;
                    case SDLK_UP:
                        Input::setButtonDown(Key::Up, state);
                        break;
                    case SDLK_LEFT:
                        Input::setButtonDown(Key::Left, state);
                        break;
                    case SDLK_RIGHT:
                        Input::setButtonDown(Key::Right, state);
                        break;
                    case SDLK_SPACE:
                        Input::setButtonDown(Key::Space, state);
                        break;
                    case SDLK_LSHIFT:
                        Input::setButtonDown(Key::LeftShift, state);
                        break;
                    case SDLK_RSHIFT:
                        Input::setButtonDown(Key::RightShift, state);
                        break;
                    case SDLK_ESCAPE:
                        if (Project::isFullscreen()) {
                            Project::setIsFullscreen(false);
                        }
                        Input::setButtonDown(Key::Escape, state);
                        break;
                    default:
                        int keyCode = (int) Event.key.keysym.sym;
                        Input::setButtonDown(keyCode, state);
                        break;
                }
            } else if (Event.type == SDL_MOUSEBUTTONDOWN) {
                bool state = true;
                if (Event.button.button == SDL_BUTTON_LEFT) {
                    Input::setButtonDown(Key::LeftMouse, state);
                    SDL_SetWindowInputFocus(sdlWindow);
                }
                if (Event.button.button == SDL_BUTTON_RIGHT) {
                    Input::setButtonDown(Key::RightMouse, state);
                }
            } else if (Event.type == SDL_MOUSEBUTTONUP) {
                bool state = false;
                if (Event.button.button == SDL_BUTTON_LEFT) {
                    Input::setButtonDown(Key::LeftMouse, state);
                }
                if (Event.button.button == SDL_BUTTON_RIGHT) {
                    Input::setButtonDown(Key::RightMouse, state);
                }
            } else if (Event.type == SDL_MOUSEMOTION) {
                Input::setMouseMovement(Event.motion.xrel, Event.motion.yrel);
                Input::setMousePosition(Event.motion.x, Event.motion.y);
            } else if (Event.type == SDL_MOUSEWHEEL) {
                Input::setMouseScroll(Event.wheel.x, Event.wheel.y);
            } else if (Event.type == SDL_WINDOWEVENT) {
                switch (Event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        this->windowWidth = Event.window.data1;
                        this->windowHeight = Event.window.data2;
                        break;
                }
            } else if (Event.type == SDL_QUIT) {
                this->shouldCloseFlag = true;
            }
        }
    }

    void SDL2Window::swapBuffers() {

    }

    std::pair<int, int> SDL2Window::getWindowDimensions() {
        return std::make_pair(this->windowWidth, this->windowHeight);
    }

    SDL_Window *SDL2Window::getSDL2Window() {
        return this->sdlWindow;
    }

    void SDL2Window::pollEditorEvents(SDL_Event &Event) {

    }
}
