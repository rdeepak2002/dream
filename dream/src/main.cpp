#include "dream/Application.h"
#include <iostream>

#ifndef EMSCRIPTEN
#include <gtest/gtest.h>
#endif

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

Dream::Application* application = nullptr;

void mainLoop() {
    #ifdef EMSCRIPTEN
    application->update();
    #else
    while (!application->shouldClose()) {
        application->update();
    }
    #endif
}

void startUpdateLoop() {
    #ifdef EMSCRIPTEN // TODO: only render on repaint call
    emscripten_set_main_loop(&mainLoop, 0, 1);
    #else
    mainLoop();
    #endif
}

int main(int ArgCount, char **Args) {
    if (ArgCount >= 2 && std::string(Args[1]) == "test") {
#ifndef EMSCRIPTEN
        ::testing::InitGoogleTest(&ArgCount, Args);
        return RUN_ALL_TESTS();
#endif
    } else {
        application = new Dream::Application();
        startUpdateLoop();
        delete application;
        return EXIT_SUCCESS;
    }
}