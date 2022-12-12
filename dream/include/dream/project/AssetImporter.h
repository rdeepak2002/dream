//
// Created by Deepak Ramalingam on 11/18/22.
//

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
