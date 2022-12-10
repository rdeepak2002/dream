//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include "dream/renderer/OpenGLRenderer.h"
#include "dream/editor/ImGuiSDL2OpenGLEditor.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include <filesystem>
#include <map>

namespace Dream {
    Application::Application() {
        this->logCollector = new LogCollector();
        Project::open(this->getResourcesRoot().append("resources").append("example-projects").append("sample-project"));
        this->window = new SDL2OpenGLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new ImGuiSDL2OpenGLEditor(this->window);
        this->editor->setLogCollector(logCollector);
    }

    Application::~Application() {
        delete this->logCollector;
        delete this->window;
        delete this->renderer;
        delete this->editor;
    }

    void Application::update() {
        this->window->pollEvents();
        this->fixedUpdate();
        std::pair<int, int> rendererViewportDimensions;
        if (fullscreen) {
            rendererViewportDimensions = this->window->getWindowDimensions();
        } else {
            rendererViewportDimensions = this->editor->getRendererViewportDimensions();
        }
        this->renderer->render(rendererViewportDimensions.first, rendererViewportDimensions.second, fullscreen);
        if (!fullscreen) {
            this->editor->update(this->window, this->renderer->getOutputRenderTexture());
        }
        this->window->swapBuffers();
    }

    bool Application::shouldClose() {
        return this->window->shouldClose();
    }

    void Application::fixedUpdate() {
        // TODO: make this actually fixed update
        Project::getScene()->fixedUpdate(1.0 / 60.0);
    }

    std::filesystem::path Application::getResourcesRoot() {
        // check if current path already has resources folder
        if (std::filesystem::exists(std::filesystem::current_path().append("resources"))) {
            return std::filesystem::current_path();
        }
        // try to find location of resources folder for desktop debug build
        auto resourcesFolder = std::filesystem::current_path()
                .append("..")
                .append("..")
                .append("..")
                .append("resources");
        if (std::filesystem::exists(resourcesFolder)) {
            return resourcesFolder.append("..");
        }
        fprintf(stderr, "Error: unable to find examples folder\n");
        exit(EXIT_FAILURE);
    }
}
