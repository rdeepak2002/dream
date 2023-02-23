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

#include "dream/window/Input.h"
#include "dream/util/Logger.h"

namespace Dream {
    bool Input::getButtonDown(int keyCode) {
        if (Input::getInstance().keyDownMap.find(keyCode) == Input::getInstance().keyDownMap.end()) {
            return false;
        }

        if (Input::getInstance().playWindowActive) {
            return Input::getInstance().keyDownMap[keyCode];
        } else {
            return false;
        }
    }

    void Input::setButtonDown(int keyCode, bool state) {
        if (Input::getInstance().playWindowActive) {
            Input::getInstance().keyDownMap[keyCode] = state;
        }
    }

    glm::vec2 Input::getMousePosition() {
        return Input::getInstance().mousePosition - Input::getInstance().editorMousePositionOffset;
    }

    void Input::setMousePosition(double x, double y) {
        if (Input::getInstance().playWindowActive) {
            Input::getInstance().mousePosition = {x, y};
        }
    }

    glm::vec2 Input::getMouseMovement() {
        if (Input::getInstance().playWindowActive) {
            return Input::getInstance().mouseMovement;
        } else {
            return {0, 0};
        }
    }

    void Input::setMouseMovement(double x, double y) {
        Input::getInstance().mouseMovement = {x, y};
    }

    glm::vec2 Input::getMouseScroll() {
        if (Input::getInstance().playWindowActive) {
            return Input::getInstance().mouseScroll;
        } else {
            return {0, 0};
        }
    }

    void Input::setMouseScroll(double x, double y) {
        Input::getInstance().mouseScroll = {x, y};
    }

    void Input::resetMouseDynamicState() {
        Input::setMouseMovement(0, 0);
        Input::setMouseScroll(0, 0);
    }

    void Input::activatePointerLock(bool enablePointerLock) {
        Input::getInstance().enablePointerLock = enablePointerLock;
    }

    bool Input::pointerLockActivated() {
        return Input::getInstance().enablePointerLock;
    }

    bool Input::isEditorRendererActive() {
        return Input::getInstance().editorRendererActive;
    }

    void Input::setEditorRendererActive(bool editorRendererActive) {
        Input::getInstance().editorRendererActive = editorRendererActive;
    }

    bool Input::isPlayWindowActive() {
        return Input::getInstance().playWindowActive;
    }

    void Input::setPlayWindowActive(bool playWindowActive) {
        Input::getInstance().playWindowActive = playWindowActive;
    }

    void Input::setEditorMousePositionOffset(double x, double y) {
        Input::getInstance().editorMousePositionOffset = {x , y};
    }

    void Input::setRendererDimensions(int width, int height) {
        Input::getInstance().rendererWidth = width;
        Input::getInstance().rendererHeight = height;
    }

    glm::vec2 Input::getRelativeMousePosition() {
        float w = (float) Input::getInstance().rendererWidth;
        float h = (float) Input::getInstance().rendererHeight;
        if (w == 0 || h == 0) {
            return {0, 0};
        }
        glm::vec2 res = Input::getMousePosition();
        res.x /= w;
        res.y /= h;
        res = res - glm::vec2{0.5, 0.5};
        res = 2.0f * res;
        return res;
    }
}
