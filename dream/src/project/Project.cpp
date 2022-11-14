//
// Created by Deepak Ramalingam on 10/29/22.
//

#include "dream/project/Project.h"

#include <utility>
#include "dream/project/OpenGLAssetImporter.h"

namespace Dream {
    Project::Project() {
        assetImporter = new OpenGLAssetImporter();
    }

    Project::~Project() {
        delete assetImporter;
    }

    void Project::open(std::filesystem::path filepath) {
        Project::getInstance().openHelper(std::move(filepath));
    }

    std::filesystem::path Project::getPath() {
        return Project::getInstance().getPathHelper();
    }

    void Project::openHelper(std::filesystem::path filepath) {
        if (std::filesystem::exists(filepath)) {
            this->path = std::move(filepath);
        } else {
            fprintf(stderr, "Error: project folder does not exist\n");
            exit(EXIT_FAILURE);
        }
    }

    std::filesystem::path Project::getPathHelper() {
        return this->path;
    }

    Scene& Project::getScene() {
        return scene;
    }

    AssetImporter *Project::getAssetImporter() {
        return assetImporter;
    }
}
