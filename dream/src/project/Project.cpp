//
// Created by Deepak Ramalingam on 10/29/22.
//

#include "dream/project/Project.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "dream/project/OpenGLAssetLoader.h"

namespace Dream {
    Project::Project() {
        assetLoader = new OpenGLAssetLoader();
        resourceManager = new ResourceManager();
        scene = new Scene();
    }

    Project::~Project() {
        delete assetLoader;
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

    AssetLoader *Project::getAssetLoaderHelper() {
        return assetLoader;
    }

    ResourceManager *Project::getResourceManagerHelper() {
        return resourceManager;
    }

    AssetLoader *Project::getAssetLoader() {
        return getInstance().getAssetLoaderHelper();
    }

    ResourceManager *Project::getResourceManager() {
        return getInstance().getResourceManagerHelper();
    }

    Scene *Project::getSceneHelper() {
        return scene;
    }

    void Project::saveScene() {
        YAML::Emitter out;
        out << YAML::BeginMap;
        Project::getInstance().scene->serialize(out);
        out << YAML::EndMap;
        std::string savePath = std::filesystem::path(Project::getPath()).append("assets").append("main.scene");
        std::ofstream fout(savePath);
        fout << out.c_str();
        std::cout << "Saved scene to " << savePath << std::endl;
    }
}
