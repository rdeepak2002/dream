//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#define WinWidth 600
#define WinHeight 400

Application::Application() {
    this->window = new SDLWindow(WinWidth, WinHeight);
    this->renderer = new OpenGLRenderer();
}

void Application::update() {
    this->window->pollEvents();
    this->fixedUpdate();
    this->renderer->render(WinWidth, WinHeight);
    this->window->swapBuffers();
}

bool Application::shouldClose() {
    return this->window->shouldClose();
}

void Application::fixedUpdate() {

}
