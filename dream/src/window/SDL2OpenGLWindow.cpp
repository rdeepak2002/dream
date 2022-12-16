//
// Created by Deepak Ramalingam on 10/24/22.
//

#include "dream/window/SDL2OpenGLWindow.h"

#include <glad/glad.h>
#include "dream/editor/ImGuiSDL2OpenGLEditor.h"

namespace Dream {
    SDL2OpenGLWindow::SDL2OpenGLWindow() : SDL2Window(SDL_WINDOW_OPENGL) {
        #ifdef EMSCRIPTEN
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #endif
        glContext = SDL_GL_CreateContext(sdlWindow);

        // Check OpenGL properties
        #ifdef EMSCRIPTEN
        gladLoadGLES2Loader(SDL_GL_GetProcAddress);
        #else
        gladLoadGLLoader(SDL_GL_GetProcAddress);
        #endif
    }

    void SDL2OpenGLWindow::swapBuffers() {
        SDL_GL_SwapWindow(sdlWindow);
    }

    SDL_GLContext SDL2OpenGLWindow::getSDL2GLContext() {
        return this->glContext;
    }

    void SDL2OpenGLWindow::pollEditorEvents(SDL_Event &Event) {
        Dream::ImGuiSDL2OpenGLEditor::pollEvents(Event);
    }
}
