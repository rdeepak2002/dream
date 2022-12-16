//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIEDITORMENU_H
#define DREAM_IMGUIEDITORMENU_H

#include <imgui/imgui.h>
#include "dream/editor/ImGuiFileBrowser.h"

namespace Dream {
    class ImGuiEditorMenu {
    public:
        ImGuiEditorMenu(ImGuiFileBrowser* fileBrowser);
        void update();
    private:
        ImGuiFileBrowser* fileBrowser;
    };
}

#endif //DREAM_IMGUIEDITORMENU_H
