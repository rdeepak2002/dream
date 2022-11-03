//
// Created by Deepak Ramalingam on 10/27/22.
//

#include "dream/ImGuiSDL2OpenGLEditor.h"

#include <iostream>
#include <imgui.h>
#include <imgui_internal.h>
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "dream/OpenGLShader.h"

namespace Dream {
    ImGuiSDL2OpenGLEditor::ImGuiSDL2OpenGLEditor(Dream::SDLWindow *window) : ImGuiEditor(window) {
        // setup platform/renderer bindings
        ImGui_ImplSDL2_InitForOpenGL(window->getSDL2Window(), window->getSDL2GLContext());
        ImGui_ImplOpenGL3_Init(OpenGLShader::getShaderVersion().c_str());
    }

    void ImGuiSDL2OpenGLEditor::newFrame(Dream::SDLWindow *window) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window->getSDL2Window());
    }

    void ImGuiSDL2OpenGLEditor::pollEvents(SDL_Event Event) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }

    void ImGuiSDL2OpenGLEditor::renderDrawData() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
