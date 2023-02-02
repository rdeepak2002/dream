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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
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
