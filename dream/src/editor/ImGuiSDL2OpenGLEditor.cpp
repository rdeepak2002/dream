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

#include "dream/editor/ImGuiSDL2OpenGLEditor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "dream/renderer/OpenGLShader.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include "dream/util/Logger.h"

namespace Dream {
    ImGuiSDL2OpenGLEditor::ImGuiSDL2OpenGLEditor(Dream::Window *window) : ImGuiEditor(window) {
        // setup platform/renderer bindings
        auto sdl2OpenGLWindow = dynamic_cast<SDL2OpenGLWindow*>(window);
        if (sdl2OpenGLWindow) {
            ImGui_ImplSDL2_InitForOpenGL(sdl2OpenGLWindow->getSDL2Window(), sdl2OpenGLWindow->getSDL2GLContext());
        } else {
            Logger::fatal("Window instance does not support OpenGL rendering");
        }
        ImGui_ImplOpenGL3_Init(OpenGLShader::getShaderVersion().c_str());
    }

    void ImGuiSDL2OpenGLEditor::newFrame(Dream::Window *window) {
        ImGui_ImplOpenGL3_NewFrame();
        auto sdl2OpenGLWindow = dynamic_cast<SDL2OpenGLWindow*>(window);
        if (sdl2OpenGLWindow) {
            ImGui_ImplSDL2_NewFrame(sdl2OpenGLWindow->getSDL2Window());
        } else {
            Logger::fatal("Window instance does not support OpenGL rendering");
        }
    }

    void ImGuiSDL2OpenGLEditor::pollEvents(SDL_Event Event) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }

    void ImGuiSDL2OpenGLEditor::renderDrawData() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
