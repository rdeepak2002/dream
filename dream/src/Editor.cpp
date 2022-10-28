//
// Created by Deepak Ramalingam on 10/27/22.
//

#include "dream/Editor.h"

#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

namespace Dream {
    Editor::Editor(Dream::SDLWindow *window) {
        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;

        // setup Dear ImGui style
        ImGui::StyleColorsDark();

        // setup platform/renderer bindings
        ImGui_ImplSDL2_InitForOpenGL(window->getSDL2Window(), window->getSDL2GLContext());
        #ifdef EMSCRIPTEN
        std::string glsl_version = "#version 300 es";
        #else
        std::string glsl_version = "#version 330 core";
        #endif
        ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    }

    void Editor::update(Dream::SDLWindow *window) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window->getSDL2Window());
        ImGui::NewFrame();
        bool show_demo_window = false;
        ImGui::ShowDemoWindow(&show_demo_window);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Editor::pollEvents(SDL_Event Event) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }
}
