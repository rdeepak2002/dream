#include "dream/Application.h"
#include <iostream>

#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

void mainLoop() {
    #ifdef EMSCRIPTEN
    update();
    #else
    auto* application = new Application();

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
    return EXIT_SUCCESS;
}