#include "dream/Application.h"
#include <iostream>

#ifdef EMSCRIPTEN
    #include "emscripten.h"
#endif

int update()
{
    auto* application = new Application();

    while (!application->shouldClose()) {
        application->update();
    }

    return EXIT_SUCCESS;
}

void mainLoop() {
    update();
}

void startUpdateLoop() {
#ifdef EMSCRIPTEN // TODO: only render on repaint call
    // Define a mail loop function, that will be called as fast as possible
    emscripten_set_main_loop(&mainLoop, 0, 1);
#else
    // This is the normal C/C++ main loop
    update();
#endif
}

int main(int ArgCount, char **Args)
{
    startUpdateLoop();
    return EXIT_SUCCESS;
}