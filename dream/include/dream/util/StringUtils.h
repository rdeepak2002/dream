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

#ifndef DREAM_STRINGUTILS_H
#define DREAM_STRINGUTILS_H

#include <iostream>

namespace Dream {
    class StringUtils {
    public:
        static std::string getFilePathRelativeToProjectFolder(std::string path);
        static std::pair<std::string, std::string> splitAndShorten(std::string str, bool ellipsis = true, int maxLength = 10, int splitLength = 5);
        static std::string shorten(std::string str, bool ellipsis = true, int maxLength = 5);
    };
}

#endif //DREAM_STRINGUTILS_H
