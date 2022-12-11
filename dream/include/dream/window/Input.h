//
// Created by Deepak Ramalingam on 11/26/22.
//

#ifndef DREAM_INPUT_H
#define DREAM_INPUT_H


#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Dream {
    class Input {
    public:
        static Input& getInstance() {
            static Input instance;
            return instance;
        }

        static bool getButtonDown(int keyCode);
        static void setButtonDown(int keyCode, bool state);

        static glm::vec2 getMousePosition();
        static void setMousePosition(double x, double y);

        static glm::vec2 getMouseMovement();
        static void setMouseMovement(double x, double y);
    private:
        Input() {}
        std::unordered_map<int, bool> keyDownMap;
        glm::vec2 mousePosition;
        glm::vec2 mouseMovement;
    public:
        Input(Input const&) = delete;
        void operator=(Input const&) = delete;
    };
}

#endif //DREAM_INPUT_H
