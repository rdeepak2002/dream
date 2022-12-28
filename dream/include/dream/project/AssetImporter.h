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

#ifndef DREAM_ASSETIMPORTER_H
#define DREAM_ASSETIMPORTER_H

#include <iostream>
#include <filesystem>

namespace Dream {
    class AssetImporter {
    public:
        /**
         * Import asset outside of project folder into it. Generate .meta files to reference files by GUID.
         * @param path
         */
        void importAsset(std::filesystem::path path);

        void createMetaFile(std::filesystem::path path);

    private:
        void importAssetHelper(std::filesystem::path rootPath, std::filesystem::path relativePath);
    };
}

#endif //DREAM_ASSETIMPORTER_H
