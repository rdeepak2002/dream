//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIEDITORMENU_H
#define DREAM_IMGUIEDITORMENU_H

#include <imgui/imgui.h>
#include <imgui-filebrowser/imfilebrowser.h>

namespace Dream {
    class ImGuiEditorMenu {
    public:
        void update(ImGui::FileBrowser *fileImporterBrowser);
    };
}

#endif //DREAM_IMGUIEDITORMENU_H
