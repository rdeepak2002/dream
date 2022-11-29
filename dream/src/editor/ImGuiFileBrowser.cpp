//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiFileBrowser.h"
#include "dream/project/Project.h"

namespace Dream {
    ImGuiFileBrowser::ImGuiFileBrowser() {
        fileImporterBrowser = nullptr;
    }

    ImGuiFileBrowser::~ImGuiFileBrowser() {
        delete fileImporterBrowser;
    }

    void ImGuiFileBrowser::update() {
        if (fileImporterBrowser) {
            fileImporterBrowser->Display();
            if (fileImporterBrowser->HasSelected()) {
                std::filesystem::path selectedFilePath = fileImporterBrowser->GetSelected();
                Project::getAssetImporter()->importAsset(selectedFilePath);
                fileImporterBrowser->ClearSelected();
            }
        }
    }

    void ImGuiFileBrowser::selectDirectory() {
        delete fileImporterBrowser;
        fileImporterBrowser = new ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
        fileImporterBrowser->SetTitle("import folder");
        fileImporterBrowser->SetPwd(Project::getPath());
        fileImporterBrowser->Open();
    }

    void ImGuiFileBrowser::selectFile() {
        delete fileImporterBrowser;
        fileImporterBrowser = new ImGui::FileBrowser();
        fileImporterBrowser->SetTitle("import file");
        fileImporterBrowser->SetPwd(Project::getPath());
        fileImporterBrowser->Open();
    }
}
