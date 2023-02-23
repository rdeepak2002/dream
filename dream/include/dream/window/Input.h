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

#ifndef DREAM_INPUT_H
#define DREAM_INPUT_H


#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Dream {
    class Input {
    public:
        static Input &getInstance() {
            static Input instance;
            return instance;
        }

        static bool getButtonDown(int keyCode);

        static void setButtonDown(int keyCode, bool state);

        static glm::vec2 getMousePosition();

        /**
         * Mouse position on a relative [-1, 1] range
         * @return 2D vector mouse position on a relative [-1, 1] range
         */
        static glm::vec2 getRelativeMousePosition();

        static void setMousePosition(double x, double y);

        static glm::vec2 getMouseMovement();

        static void setMouseMovement(double x, double y);

        static glm::vec2 getMouseScroll();

        static void setMouseScroll(double x, double y);

        static void resetMouseDynamicState();

        static void activatePointerLock(bool enablePointerLock);

        static bool pointerLockActivated();

        static bool isEditorRendererActive();

        static void setEditorRendererActive(bool editorRendererActive);

        static bool isPlayWindowActive();

        static void setPlayWindowActive(bool playWindowActive);

        static void setEditorMousePositionOffset(double x, double y);

        static void setRendererDimensions(int width, int height);

    private:
        Input() {}

        std::unordered_map<int, bool> keyDownMap;
        glm::vec2 mousePosition;
        glm::vec2 mouseMovement;
        glm::vec2 mouseScroll;
        glm::vec2 editorMousePositionOffset = glm::vec2(0, 0);    // position of the top left of the editor
        int rendererWidth = 1;
        int rendererHeight = 1;
        bool enablePointerLock = false;
        bool editorRendererActive = false;
        bool playWindowActive = false;
    public:
        Input(Input const &) = delete;

        void operator=(Input const &) = delete;
    };
}

#endif //DREAM_INPUT_H
