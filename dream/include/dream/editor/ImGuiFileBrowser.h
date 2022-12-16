//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIFILEBROWSER_H
#define DREAM_IMGUIFILEBROWSER_H

#include <imgui/imgui.h>
#include <imgui-filebrowser/imfilebrowser.h>

namespace Dream {
    class ImGuiFileBrowser {
    public:
        ImGuiFileBrowser();
        ~ImGuiFileBrowser();
        void update();
        void selectDirectory();
        void selectFile();
    private:
        ImGui::FileBrowser* fileImporterBrowser;
    };
}

#endif //DREAM_IMGUIFILEBROWSER_H
