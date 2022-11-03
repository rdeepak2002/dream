//
// Created by Deepak Ramalingam on 10/21/22.
//

#ifndef DREAM_APPLICATION_H
#define DREAM_APPLICATION_H

#include "dream/OpenGLRenderer.h"
#include "dream/editor/ImguiEditor.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include "dream/Project.h"

namespace Dream {
    class Application {
    public:
        Application();
        ~Application();
        void update();
        bool shouldClose();
    private:
        SDL2OpenGLWindow* window;
        OpenGLRenderer* renderer;
        ImGuiEditor* editor;
        void fixedUpdate();
        std::filesystem::path getResourcesRoot();
    };
}

#endif //DREAM_APPLICATION_H
