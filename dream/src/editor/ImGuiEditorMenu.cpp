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

