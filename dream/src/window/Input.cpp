//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/window/Input.h"
#include "dream/util/Logger.h"

namespace Dream {
    bool Input::getButtonDown(int keyCode) {
        if (Input::getInstance().keyDownMap.find(keyCode) == Input::getInstance().keyDownMap.end()) {
            return false;
        }

        return Input::getInstance().keyDownMap[keyCode];
    }

    void Input::setButtonDown(int keyCode, bool state) {
        Input::getInstance().keyDownMap[keyCode] = state;
    }

    glm::vec2 Input::getMousePosition() {
        return Input::getInstance().mousePosition;
    }

    void Input::setMousePosition(double x, double y) {
        Input::getInstance().mousePosition = {x, y};
    }

    glm::vec2 Input::getMouseMovement() {
        return Input::getInstance().mouseMovement;
    }

    void Input::setMouseMovement(double x, double y) {
        Input::getInstance().mouseMovement = {x, y};
    }

    glm::vec2 Input::getMouseScroll() {
        return Input::getInstance().mouseScroll;
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
}
