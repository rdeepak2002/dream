//
// Created by Deepak Ramalingam on 10/27/22.
//

#include "dream/editor/ImGuiSDL2OpenGLEditor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "dream/OpenGLShader.h"
#include "dream/window/SDL2OpenGLWindow.h"

namespace Dream {
    ImGuiSDL2OpenGLEditor::ImGuiSDL2OpenGLEditor(Dream::Window *window) : ImGuiEditor(window) {
        // setup platform/renderer bindings
        auto sdl2OpenGLWindow = dynamic_cast<SDL2OpenGLWindow*>(window);
        if (sdl2OpenGLWindow) {
            ImGui_ImplSDL2_InitForOpenGL(sdl2OpenGLWindow->getSDL2Window(), sdl2OpenGLWindow->getSDL2GLContext());
        } else {
            printf("Window instance does not support OpenGL rendering");
            exit(EXIT_FAILURE);
        }
        ImGui_ImplOpenGL3_Init(OpenGLShader::getShaderVersion().c_str());
    }

    void ImGuiSDL2OpenGLEditor::newFrame(Dream::Window *window) {
        ImGui_ImplOpenGL3_NewFrame();
        auto sdl2OpenGLWindow = dynamic_cast<SDL2OpenGLWindow*>(window);
        if (sdl2OpenGLWindow) {
            ImGui_ImplSDL2_NewFrame(sdl2OpenGLWindow->getSDL2Window());
        } else {
            printf("Window instance does not support OpenGL rendering");
            exit(EXIT_FAILURE);
        }
    }

    void ImGuiSDL2OpenGLEditor::pollEvents(SDL_Event Event) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }

    void ImGuiSDL2OpenGLEditor::renderDrawData() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
