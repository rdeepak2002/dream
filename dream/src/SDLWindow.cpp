//
// Created by Deepak Ramalingam on 10/24/22.
//

#include "dream/SDLWindow.h"

#include <glad/glad.h>

SDLWindow::SDLWindow(int windowWidth, int windowHeight) {
    // OPENGL VERSION
    auto WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    WindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
    this->Window = SDL_CreateWindow("OpenGL Test", 0, 0, windowWidth, windowHeight, WindowFlags);

#ifdef EMSCRIPTEN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif
    SDL_GLContext Context = SDL_GL_CreateContext(Window);

    // Check OpenGL properties
    printf("Going to load Glad function pointers\n");
    #ifdef EMSCRIPTEN
    gladLoadGLES2Loader(SDL_GL_GetProcAddress);
    #else
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    #endif
    printf("Loaded Glad function pointers\n");

    this->shouldCloseFlag = false;
}

void SDLWindow::pollEvents() {
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
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

