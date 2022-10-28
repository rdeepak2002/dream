//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include <iostream>

namespace Dream {
    Application::Application() {
        this->window = new SDLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new Editor(this->window);
    }

    Application::~Application() {
        delete this->window;
        delete this->renderer;
        delete this->editor;
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
}
