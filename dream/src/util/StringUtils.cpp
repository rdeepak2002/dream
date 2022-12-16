//
// Created by Deepak Ramalingam on 12/16/22.
//

#include "dream/util/StringUtils.h"
#include "dream/project/Project.h"

namespace Dream {
    std::string StringUtils::getFilePathRelativeToProjectFolder(std::string path) {
        std::string projectFolderPath = Project::getPath();
        size_t found = path.find(projectFolderPath);
        if (found != std::string::npos) {
            return path.substr(found + projectFolderPath.length());
        }
        return path;
    }
}
