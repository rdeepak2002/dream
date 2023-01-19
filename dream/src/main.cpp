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
#include <quickjs.h>
#include <quickjs-libc.h>

static JSValue js_print(JSContext *ctx, JSValueConst this_val,
                        int argc, JSValueConst *argv)
{
    int i;
    const char *str;

    for(i = 0; i < argc; i++) {
        if (i != 0)
            putchar(' ');
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        fputs(str, stdout);
        JS_FreeCString(ctx, str);
    }
    putchar('\n');
    return JS_UNDEFINED;
}

void test_quickjs() {
    JSRuntime* rt; JSContext* ctx;

    rt = JS_NewRuntime();
    ctx = JS_NewContextRaw(rt);

    JS_AddIntrinsicBaseObjects(ctx);
    JS_AddIntrinsicEval(ctx);

    JSValue global_obj, console;

    global_obj = JS_GetGlobalObject(ctx);

    console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log",
                      JS_NewCFunction(ctx, js_print, "log", 1));
    JS_SetPropertyStr(ctx, global_obj, "console", console);

    JS_FreeValue(ctx, global_obj);

//    const char source[] = "console.log(Math.pow(12, 2));100";
    auto source = "console.log(Math.pow(12, 2));";
    size_t len = sizeof(source);

    printf("Evaluate: %s length: %i\n", source, len);

    JSValue ret = JS_Eval(ctx,
                          source,
                          len,
                          "<evalScript>",
                          JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(ret)) {
        js_std_dump_error(ctx);
        JS_ResetUncatchableError(ctx);
    }

    JSValue d_JS = __JS_NewFloat64(ctx, 12.5);

    int x = JS_VALUE_GET_INT(ret);

    printf("Int: %i \n", x);
    printf("Double: %f \n", JS_VALUE_GET_FLOAT64(d_JS));

    JSValue i_JS = JS_NewInt32(ctx, 1);
    JS_ToInt32(ctx, &x, i_JS);

    printf("1 Int: %i \n", x);

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    printf("Hello, C!\n");
}

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
    test_quickjs();

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