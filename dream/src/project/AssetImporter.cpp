//
// Created by Deepak Ramalingam on 11/18/22.
//

#include <utility>
#include <fstream>
#include "dream/project/AssetImporter.h"
#include "dream/project/Project.h"
#include "dream/util/IDUtils.h"

namespace Dream {
    void AssetImporter::importAsset(std::filesystem::path path) {
        importAssetHelper(std::move(path.parent_path()), path.filename());
    }

    void AssetImporter::importAssetHelper(std::filesystem::path rootPath, std::filesystem::path relativePath) {
        std::filesystem::path combinedPath = std::filesystem::path(rootPath);
        combinedPath += std::filesystem::path::preferred_separator;
        combinedPath += relativePath;

        if (!std::filesystem::exists(combinedPath)) {
            std::cout << "Directory or file does not exist: " << combinedPath << std::endl;
            exit(EXIT_FAILURE);
        }

        if (std::filesystem::is_directory(combinedPath)) {
            auto new_directory_folder = Project::getPath().append("assets").append(relativePath.c_str());
            // create directory
            std::filesystem::create_directory(new_directory_folder);
//            std::cout << "will create new directory: " << new_directory_folder << std::endl;
            for (const auto & entry : std::filesystem::directory_iterator(combinedPath)) {
                std::filesystem::path newRelativePath = std::filesystem::path(relativePath);
                newRelativePath += std::filesystem::path::preferred_separator;
                newRelativePath += entry.path().filename();
                importAssetHelper(rootPath, newRelativePath);
            }
        } else {
            std::string fileName = combinedPath.filename();
            auto copyFrom = std::filesystem::path(combinedPath);
            auto copyTo = Project::getPath().append("assets").append(relativePath.c_str());
            auto metaFilePath = std::filesystem::path(std::string(copyTo.c_str()) + ".meta");
            // copy file
            if (std::filesystem::exists(copyTo)) {
                if (!std::filesystem::remove(copyTo)) {
                    std::cout << "Error removing file " << copyTo << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            std::filesystem::copy_file(copyFrom, copyTo);
            // create meta file
            YAML::Emitter out;
            out << YAML::BeginMap;
            out << YAML::Key << "guid" << YAML::Value << IDUtils::newGUID();
            out << YAML::EndMap;
            std::ofstream fout(metaFilePath);
            fout << out.c_str();
//            std::cout << "copy from: " << copyFrom << std::endl;
//            std::cout << "copy to: " << copyTo << std::endl;
//            std::cout << "meta file path: " << metaFilePath << std::endl;
        }
    }
}
