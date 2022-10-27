#include "dream/Application.h"
#include <iostream>

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

auto* application = new Application();

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

int main(int ArgCount, char **Args)
{
    startUpdateLoop();
    delete application;
    return EXIT_SUCCESS;
}