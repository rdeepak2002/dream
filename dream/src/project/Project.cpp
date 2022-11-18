//
// Created by Deepak Ramalingam on 10/29/22.
//

#include "dream/project/Project.h"

#include <utility>
#include "dream/project/OpenGLAssetImporter.h"

namespace Dream {
    Project::Project() {
        assetImporter = new OpenGLAssetImporter();
        resourceManager = new ResourceManager();
        scene = new Scene();
    }

    Project::~Project() {
        delete assetImporter;
        delete resourceManager;
        delete scene;
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

    Scene *Project::getScene() {
        return getInstance().scene;
    }

    AssetImporter *Project::getAssetImporterHelper() {
        return assetImporter;
    }

    ResourceManager *Project::getResourceManagerHelper() {
        return resourceManager;
    }

    AssetImporter *Project::getAssetImporter() {
        return getInstance().getAssetImporterHelper();
    }

    ResourceManager *Project::getResourceManager() {
        return getInstance().getResourceManagerHelper();
    }

    Scene *Project::getSceneHelper() {
        return scene;
    }
}
