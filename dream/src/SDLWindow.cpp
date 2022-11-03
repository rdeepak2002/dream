//
// Created by Deepak Ramalingam on 10/24/22.
//

#include "dream/SDLWindow.h"

#include <glad/glad.h>
#include <iostream>
#include "dream/ImGuiSDL2OpenGLEditor.h"

namespace Dream {
    SDLWindow::SDLWindow() {
        this->windowWidth = 1024;
        this->windowHeight = 768;

        auto WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
        this->Window = SDL_CreateWindow("Dream", 0, 0, this->windowWidth, this->windowHeight, WindowFlags);

        #ifdef EMSCRIPTEN
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #endif
        glContext = SDL_GL_CreateContext(Window);

        // Check OpenGL properties
        #ifdef EMSCRIPTEN
        gladLoadGLES2Loader(SDL_GL_GetProcAddress);
        #else
        gladLoadGLLoader(SDL_GL_GetProcAddress);
        #endif

        this->shouldCloseFlag = false;
    }

    SDLWindow::~SDLWindow() {
        SDL_DestroyWindow(Window);
    }

    void SDLWindow::pollEvents() {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            Dream::ImGuiSDL2OpenGLEditor::pollEvents(Event);
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

    void SDLWindow::swapBuffers() {
        SDL_GL_SwapWindow(Window);
    }

    bool SDLWindow::shouldClose() {
        return this->shouldCloseFlag;
    }

    std::pair<int, int> SDLWindow::getWindowDimensions() {
        return std::make_pair(this->windowWidth, this->windowHeight);
    }

    SDL_Window *SDLWindow::getSDL2Window() {
        return this->Window;
    }

    SDL_GLContext SDLWindow::getSDL2GLContext() {
        return this->glContext;
    }
}
