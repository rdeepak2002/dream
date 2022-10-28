//
// Created by Deepak Ramalingam on 10/21/22.
//

#ifndef DREAM_APPLICATION_H
#define DREAM_APPLICATION_H

#include "dream/SDLWindow.h"
#include "dream/OpenGLRenderer.h"
#include "dream/Editor.h"

namespace Dream {
    class Application {
    public:
        Application();
        ~Application();
        void update();
        bool shouldClose();
    private:
        SDLWindow* window;
        OpenGLRenderer* renderer;
        Editor* editor;
        void fixedUpdate();
    };
}

#endif //DREAM_APPLICATION_H
