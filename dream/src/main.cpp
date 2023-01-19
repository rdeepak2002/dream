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

#include "dream/Application.h"
#include <iostream>
#include <duktape.h>

#ifndef EMSCRIPTEN

#include <gtest/gtest.h>

#endif

#ifdef EMSCRIPTEN
#include "emscripten.h"
#include "dream/window/Input.h"

extern "C" {
    void disablePointerLock() {
        Dream::Input::activatePointerLock(false);
    }
}
#endif

Dream::Application *application = nullptr;

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
    duk_context *ctx = duk_create_heap_default();
    duk_eval_string(ctx, "1+2");
    printf("1+2=%d\n", (int) duk_get_int(ctx, -1));
    duk_destroy_heap(ctx);

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