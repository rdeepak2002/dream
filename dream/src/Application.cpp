//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include <iostream>
#include <filesystem>

namespace Dream {
    Application::Application() {
        this->window = new SDLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new Editor(this->window);
        this->project = new Project(this->getResourcesRoot().append("examples").append("hello_world"));
    }

    Application::~Application() {
        delete this->window;
        delete this->renderer;
        delete this->editor;
        delete this->project;
    }

    void Application::update() {
        this->window->pollEvents();
        std::pair<int, int> windowDimensions = this->window->getWindowDimensions();
        this->fixedUpdate();
        this->renderer->render(windowDimensions.first, windowDimensions.second);
        this->editor->update(this->window);
        this->window->swapBuffers();
    }

    bool Application::shouldClose() {
        return this->window->shouldClose();
    }

    void Application::fixedUpdate() {

    }

    std::filesystem::path Application::getResourcesRoot() {
        if (std::filesystem::exists(std::filesystem::current_path().append("examples"))) {
            return std::filesystem::current_path();
        }
        // try to find location of examples folder for desktop debug build
        auto examplesFolder = std::filesystem::current_path()
                .append("..")
                .append("..")
                .append("..")
                .append("examples");
        if (std::filesystem::exists(examplesFolder)) {
            return examplesFolder.append("..");
        }
        fprintf(stderr, "Error: unable to find examples folder\n");
        exit(EXIT_FAILURE);
    }
}
