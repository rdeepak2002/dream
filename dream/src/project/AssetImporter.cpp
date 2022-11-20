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
            if (!std::filesystem::exists(new_directory_folder)) {
                std::filesystem::create_directory(new_directory_folder);
            }
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
            if (copyTo != copyFrom) {
                std::filesystem::copy_file(copyFrom, copyTo, std::filesystem::copy_options::update_existing);
                // create meta file
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "guid" << YAML::Value << IDUtils::newGUID();
                out << YAML::EndMap;
                std::ofstream fout(metaFilePath);
                fout << out.c_str();
            } else {
                std::cout << "WARNING: Ignoring importing of " << copyFrom << std::endl;
            }
        }
    }
}
