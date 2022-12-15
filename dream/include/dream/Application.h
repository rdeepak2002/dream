//
// Created by Deepak Ramalingam on 10/21/22.
//

#ifndef DREAM_APPLICATION_H
#define DREAM_APPLICATION_H

#include "dream/renderer/Renderer.h"
#include "dream/editor/Editor.h"
#include "dream/window/Window.h"
#include "dream/project/Project.h"
#include "dream/editor/LogCollector.h"

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);

namespace Dream {
    class Application {
    public:
        Application();
        ~Application();
        void update();
        bool shouldClose();
//        float getCurrentTime();
    private:
        bool fullscreen = false;
        LogCollector* logCollector;
        Window* window;
        Renderer* renderer;
        Editor* editor;
        void fixedUpdate();
        std::filesystem::path getResourcesRoot();
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
        std::chrono::nanoseconds lag;
//        float deltaTime;
    };
}

#endif //DREAM_APPLICATION_H
