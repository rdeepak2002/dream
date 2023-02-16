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
#include "dream/project/Project.h"
#include <fstream>
#include <iostream>
#include <quickjs.h>
#include <quickjs-libc.h>
#include <quickjspp/quickjspp.hpp>

class MyClass
{
public:
    MyClass() {}
    MyClass(std::vector<int>) {}

    double member_variable = 5.5;
    std::string member_function(const std::string& s) { return "Hello, " + s; }
};

void println(qjs::rest<std::string> args) {
    for (auto const & arg : args) std::cout << arg << " ";
    std::cout << "\n";
}

int test_quickjs() {
    qjs::Runtime runtime;
    qjs::Context context(runtime);
    try
    {
        // export classes as a module
        auto& module = context.addModule("MyModule");
        module.function<&println>("println");
        module.class_<MyClass>("MyClass")
                .constructor<>()
                .constructor<std::vector<int>>("MyClassA")
                .fun<&MyClass::member_variable>("member_variable")
                .fun<&MyClass::member_function>("member_function");
        // import module
        context.eval(R"xxx(
            import * as my from 'MyModule';
            globalThis.my = my;
        )xxx", "<import>", JS_EVAL_TYPE_MODULE);
        // evaluate js code
        context.eval(R"xxx(
            let v1 = new my.MyClass();
            v1.member_variable = 1;
            let v2 = new my.MyClassA([1,2,3]);
            function my_callback(str) {
              my.println("at callback:", v2.member_function(str));
            }
        )xxx");

        // callback
        auto cb = (std::function<void(const std::string&)>) context.eval("my_callback");
        cb("world");
    }
    catch(qjs::exception)
    {
        auto exc = context.getException();
        std::cerr << (std::string) exc << std::endl;
        if((bool) exc["stack"])
            std::cerr << (std::string) exc["stack"] << std::endl;
        return 1;
    }
    return 0;
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

// TODO: only define whith emscripten build
extern "C" {
    int load_file_return(char const *filename, char *buffer, size_t buffer_size) {
        auto incomingFilepath = std::filesystem::path(filename);
        if (!std::filesystem::exists("web_tmp")) {
            std::filesystem::create_directory("web_tmp");
        }
        if (!std::filesystem::exists(incomingFilepath.parent_path())) {
            std::filesystem::create_directory(incomingFilepath.parent_path());
        }
        std::cout << "Received file " << filename << std::endl;

        std::string_view stringViewBuffer = {buffer, buffer_size};
        std::cout << "string view stuff: " << stringViewBuffer << std::endl;

        std::ofstream fileOutputStream(incomingFilepath);
        fileOutputStream << stringViewBuffer;
        fileOutputStream.close();

        return 0;
    }
}

// TODO: only define whith emscripten build
extern "C" {
    int import_temporary_web_assets() {
        auto webTemporaryDirectory = std::filesystem::path("web_tmp");

        if (std::filesystem::exists(webTemporaryDirectory)) {
            for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(webTemporaryDirectory)) {
                Dream::Project::getAssetImporter()->importAsset(dirEntry);
            }
//            std::filesystem::remove(webTemporaryDirectory);
        } else {
            std::cout << "Web temporary directory not found" << std::endl;
        }

        return 0;
    }
}

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
    if (ArgCount >= 2 && std::string(Args[1]) == "test") {
#ifndef EMSCRIPTEN
        ::testing::InitGoogleTest(&ArgCount, Args);
        return RUN_ALL_TESTS();
#endif
    } else {
        test_quickjs();
        application = new Dream::Application();
        startUpdateLoop();
        delete application;
        return EXIT_SUCCESS;
    }
}