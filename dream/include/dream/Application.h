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

    private:
        LogCollector *logCollector;
        Window *window;
        Renderer *renderer;
        Editor *editor;

        void fixedUpdate();

        std::filesystem::path getResourcesRoot();

        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
        std::chrono::nanoseconds lag;
    };
}

#endif //DREAM_APPLICATION_H
