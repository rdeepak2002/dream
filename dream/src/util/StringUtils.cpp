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

    std::string StringUtils::shorten(std::string str, bool ellipsis, int maxLength) {
        if (str.length() > maxLength) {
            if (ellipsis) {
                return str.substr(0, maxLength - 3) + "...";
            } else {
                return str.substr(0, maxLength);
            }
        }
        return str;
    }

    std::pair<std::string, std::string>
    StringUtils::splitAndShorten(std::string str, bool ellipsis, int maxLength, int splitLength) {
        if (str.length() > maxLength) {
            int strLen = (int) str.length();
            std::string firstHalf = str.substr(0, strLen / 2);
            std::string secondHalf = str.substr(strLen / 2);
            firstHalf = firstHalf.substr(0, maxLength / 2 - 1 - 2);
            secondHalf = "..." + secondHalf.substr(maxLength / 2 - 1 - 1);
            return {firstHalf, secondHalf};
        } else if (str.length() <= maxLength && str.length() > splitLength) {
            int strLen = (int) str.length();
            std::string firstHalf = str.substr(0, strLen / 2);
            std::string secondHalf = str.substr(strLen / 2);
            return {firstHalf, secondHalf};
        }
        return {str, ""};
    }
}
