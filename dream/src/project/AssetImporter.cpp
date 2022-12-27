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
#include <utility>
#include <fstream>
#include "dream/project/AssetImporter.h"
#include "dream/project/Project.h"
#include "dream/util/IDUtils.h"
#include "dream/util/Logger.h"

namespace Dream {
    void AssetImporter::importAsset(std::filesystem::path path) {
        importAssetHelper(std::move(path.parent_path()), path.filename());
    }

    void AssetImporter::importAssetHelper(std::filesystem::path rootPath, std::filesystem::path relativePath) {
        std::filesystem::path combinedPath = std::filesystem::path(rootPath);
        combinedPath += std::filesystem::path::preferred_separator;
        combinedPath += relativePath;

        if (!std::filesystem::exists(combinedPath)) {
            Logger::fatal("Directory or file does not exist: " + combinedPath.string());
        }

        if (std::filesystem::is_directory(combinedPath)) {
            auto new_directory_folder = Project::getPath().append("assets").append(relativePath.c_str());
            // create directory
            if (!std::filesystem::exists(new_directory_folder)) {
                std::filesystem::create_directory(new_directory_folder);
            }
            for (const auto &entry: std::filesystem::directory_iterator(combinedPath)) {
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
            if (combinedPath.extension() != ".meta") {
                // copy file
                if (copyTo != copyFrom) {
                    std::filesystem::copy_file(copyFrom, copyTo, std::filesystem::copy_options::update_existing);
                }
                // create meta file
                if (!std::filesystem::exists(metaFilePath)) {
                    YAML::Emitter out;
                    out << YAML::BeginMap;
                    out << YAML::Key << "guid" << YAML::Value << IDUtils::newGUID();
                    out << YAML::EndMap;
                    std::ofstream fout(metaFilePath);
                    fout << out.c_str();
                }
            }
        }

        Project::recognizeResources();
    }

    void AssetImporter::createMetaFile(std::filesystem::path path) {
        std::string fileName = path.filename();
        auto metaFilePath = std::filesystem::path(std::string(path.c_str()) + ".meta");
        if (!std::filesystem::exists(metaFilePath)) {
            YAML::Emitter out;
            out << YAML::BeginMap;
            out << YAML::Key << "guid" << YAML::Value << IDUtils::newGUID();
            out << YAML::EndMap;
            std::ofstream fout(metaFilePath);
            fout << out.c_str();
        }
    }
}
