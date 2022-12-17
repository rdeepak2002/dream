//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include "dream/renderer/OpenGLRenderer.h"
#include "dream/editor/ImGuiSDL2OpenGLEditor.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include "dream/util/Logger.h"
#include <filesystem>
#include <map>

namespace Dream {
    using clock = std::chrono::high_resolution_clock;

    Application::Application() {
        this->logCollector = new LogCollector();
        Project::open(this->getResourcesRoot().append("resources").append("example-projects").append("sample-project"));
        this->window = new SDL2OpenGLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new ImGuiSDL2OpenGLEditor(this->window);
        this->editor->setLogCollector(logCollector);
//        this->startTime = std::chrono::high_resolution_clock::now();
        this->currentTime = clock::now();
    }

    Application::~Application() {
        delete this->logCollector;
        delete this->window;
        delete this->renderer;
        delete this->editor;
    }

    void Application::update() {
        auto deltaTime = clock::now() - currentTime;
        this->currentTime = clock::now();
        this->lag += std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime);
        float dt = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count() * 0.001f;
        // update startup logo and other window-specific logic
        this->window->update(dt);
        // poll for input
        this->window->pollEvents();
        // fixed update (physics, scripts, etc.)
        while (lag >= timestep) {
            this->fixedUpdate();
            lag -= timestep;
        }
        // not fixed update (ex: animations)
        Project::getScene()->update(dt);
        // render
        std::pair<int, int> rendererViewportDimensions;
        if (Project::isFullscreen()) {
            rendererViewportDimensions = this->window->getWindowDimensions();
        } else {
            rendererViewportDimensions = this->editor->getRendererViewportDimensions();
        }
        this->renderer->render(rendererViewportDimensions.first, rendererViewportDimensions.second, Project::isFullscreen());
        if (!Project::isFullscreen()) {
            // TODO: create fixed update for editor for more costly computations
            this->editor->update(this->window, this->renderer->getOutputRenderTexture());
        }
        this->window->swapBuffers();
        this->window->setIsLoading(false);
    }

    bool Application::shouldClose() {
        return this->window->shouldClose();
    }

    void Application::fixedUpdate() {
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
        Logger::fatal("Unable to find examples folder");
        return {};
    }
}
