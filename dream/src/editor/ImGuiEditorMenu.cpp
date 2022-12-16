//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiEditorMenu.h"
#include "dream/project/Project.h"

namespace Dream {
    ImGuiEditorMenu::ImGuiEditorMenu(ImGuiFileBrowser* fileBrowser) {
        this->fileBrowser = fileBrowser;
    }

    void ImGuiEditorMenu::update() {
        if(ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Save")) {
                    Project::saveScene();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Assets")) {
                if(ImGui::MenuItem("Import Folder")) {
                    this->fileBrowser->selectDirectory();
//                    delete fileImporterBrowser;
//                    fileImporterBrowser = new ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
//                    fileImporterBrowser->SetTitle("import folder");
//                    fileImporterBrowser->SetPwd(Project::getPath());
//                    fileImporterBrowser->Open();
                }
                if(ImGui::MenuItem("Import File")) {
                    this->fileBrowser->selectFile();
//                    delete fileImporterBrowser;
//                    fileImporterBrowser = new ImGui::FileBrowser();
//                    fileImporterBrowser->SetTitle("import file");
//                    fileImporterBrowser->SetPwd(Project::getPath());
//                    fileImporterBrowser->Open();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}

