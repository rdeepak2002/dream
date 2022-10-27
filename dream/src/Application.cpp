//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"
#include <iostream>

Application::Application() {
    this->window = new SDLWindow();
    this->renderer = new OpenGLRenderer();
}

Application::~Application() {
    delete this->window;
    delete this->renderer;
}

void Application::update() {
    this->window->pollEvents();
    std::pair<int, int> windowDimensions = this->window->getWindowDimensions();
    this->fixedUpdate();
    this->renderer->render(windowDimensions.first, windowDimensions.second);
    this->window->swapBuffers();
}

bool Application::shouldClose() {
    return this->window->shouldClose();
}

void Application::fixedUpdate() {

}
