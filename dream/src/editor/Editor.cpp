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

#include "dream/editor/Editor.h"

namespace Dream {
    void Editor::update(Dream::Window *window, unsigned int frameBufferTexture) {

    }

    std::pair<int, int> Editor::getRendererViewportDimensions() {
        return std::pair<int, int>(0, 0);
    }

    Editor::Editor(Dream::Window *window) {

    }

    void Editor::newFrame(Dream::Window *window) {

    }

    void Editor::renderDrawData() {

    }

    void Editor::setLogCollector(LogCollector* logCollector) {
        this->logCollector = logCollector;
    }
}
