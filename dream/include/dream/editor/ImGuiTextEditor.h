//
// Created by Deepak Ramalingam on 11/27/22.
//

#ifndef DREAM_IMGUITEXTEDITOR_H
#define DREAM_IMGUITEXTEDITOR_H

#include <iostream>
#include <imgui.h>
#include <imgui-filebrowser/imfilebrowser.h>
#include <imgui-texteditor/TextEditor.h>

namespace Dream {
    class ImGuiTextEditor {
    public:
        ImGuiTextEditor();
        ~ImGuiTextEditor();
        void open(const std::string& filepath);
        void save();
        void loadEmptyFile();
        void update();
        void setVisibility(bool isVisible);
    private:
        bool visible;
        bool unsavedChanges;
        bool justLoaded;
        TextEditor* textEditor;
        ImGui::FileBrowser* fileBrowser;
        std::string filename;
        std::filesystem::path path;
    };
}

#endif //DREAM_IMGUITEXTEDITOR_H
