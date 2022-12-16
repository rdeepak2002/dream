//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIEDITORCONSOLEVIEW_H
#define DREAM_IMGUIEDITORCONSOLEVIEW_H

#include "dream/editor/LogCollector.h"

namespace Dream {
    class ImGuiEditorConsoleView {
    public:
        void update(LogCollector* logCollector);
    };
}

#endif //DREAM_IMGUIEDITORCONSOLEVIEW_H
