//
// Created by Deepak Ramalingam on 11/3/22.
//

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
