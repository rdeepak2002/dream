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
