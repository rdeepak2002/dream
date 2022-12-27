/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

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

    void Window::setIsLoading(bool isLoading) {
        this->isLoading = isLoading;
    }

    void Window::update(float dt) {

    }
}
