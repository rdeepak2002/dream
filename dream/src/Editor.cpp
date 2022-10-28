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
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
        bool show_demo_window = false;
        bool show_another_window = true;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window->getSDL2Window());
        ImGui::NewFrame();

        // 1. Show demo window
        ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Editor::pollEvents(SDL_Event Event) {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }
}
