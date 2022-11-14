//
// Created by Deepak Ramalingam on 10/21/22.
//

#ifndef DREAM_APPLICATION_H
#define DREAM_APPLICATION_H

#include "dream/renderer/Renderer.h"
#include "dream/editor/Editor.h"
#include "dream/window/Window.h"
#include "dream/project/Project.h"

namespace Dream {
    class Application {
    public:
        Application();
        ~Application();
        void update();
        bool shouldClose();
    private:
        bool fullscreen = false;
        Window* window;
        Renderer* renderer;
        Editor* editor;
        void fixedUpdate();
        std::filesystem::path getResourcesRoot();
    };
}

#endif //DREAM_APPLICATION_H
