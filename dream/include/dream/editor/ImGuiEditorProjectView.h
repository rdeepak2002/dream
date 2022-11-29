//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIEDITORPROJECTVIEW_H
#define DREAM_IMGUIEDITORPROJECTVIEW_H

#include <filesystem>
#include <set>

namespace Dream {
    class ImGuiEditorProjectView {
    public:
        ImGuiEditorProjectView();
        void update();
    private:
        std::filesystem::path currentPath;
        unsigned int fileIcon, folderIcon;
        std::set<std::string> ignoredExtensions;
        std::set<std::string> ignoredFileNames;
        std::vector<std::string> split(const std::string& str, const char& ch);
    };
}

#endif //DREAM_IMGUIEDITORPROJECTVIEW_H
