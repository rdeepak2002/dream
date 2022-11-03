//
// Created by Deepak Ramalingam on 11/3/22.
//

#include "dream/window/Window.h"

namespace Dream {
    void Window::pollEvents() {

    }

    void Window::swapBuffers() {

    }

    bool Window::shouldClose() const {
        return this->shouldCloseFlag;
    }

    std::pair<int, int> Window::getWindowDimensions() {
        return std::make_pair(0, 0);
    }

    Window::Window() {
        this->shouldCloseFlag = false;
    }
}
