//
// Created by Deepak Ramalingam on 10/29/22.
//

#include "dream/project/Project.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "dream/project/OpenGLAssetLoader.h"
#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"
#include "dream/window/Input.h"

namespace Dream {
    Project::Project() {
        assetLoader = new OpenGLAssetLoader();
        resourceManager = new ResourceManager();
        scene = new Scene();
        playing = false;
        fullscreen = false;
    }

    Project::~Project() {
        delete assetLoader;
        delete resourceManager;
        delete scene;
    }

    void Project::open(std::filesystem::path filepath) {
        Project::getInstance().openHelper(std::move(filepath));
    }

    void Project::recognizeResourcesHelper() {
        for (const auto& file : std::filesystem::recursive_directory_iterator(Project::getPath().append("assets"))) {
            std::string metaExtension = ".meta";
            if (!std::filesystem::is_directory(file) && file.path().extension() == metaExtension) {
                std::string originalFilePath = file.path().string().substr(0, file.path().string().length() - metaExtension.length());
                YAML::Node doc = YAML::LoadFile(file.path());
                auto guid = doc["guid"].as<std::string>();
                Project::getResourceManager()->setFilePathFromGUID(guid, originalFilePath);
            }
        }
    }

    std::filesystem::path Project::getPath() {
        return Project::getInstance().getPathHelper();
    }

    void Project::openHelper(std::filesystem::path filepath) {
        if (std::filesystem::exists(filepath)) {
            this->path = std::move(filepath);
            this->recognizeResourcesHelper();
            this->loadScene();
        } else {
            Logger::fatal("Project folder does not exist");
        }
    }

    void Project::saveScene() {
        YAML::Emitter out;
        out << YAML::BeginMap;
        Project::getInstance().scene->serialize(out);
        out << YAML::EndMap;
        std::string savePath = std::filesystem::path(Project::getPath()).append("assets").append("main.scene");
        std::ofstream fout(savePath);
        fout << out.c_str();
        Logger::info("Saved scene to " + savePath);
    }

    void Project::loadScene() {
        std::string loadPath = std::filesystem::path(Project::getPath()).append("assets").append("main.scene");
        YAML::Node doc = YAML::LoadFile(loadPath);
        auto entitiesYaml = doc["Entities"].as<std::vector<YAML::Node>>();
        for (const YAML::Node& entityYaml : entitiesYaml) {
            bool isRootEntity = entityYaml[Component::RootComponent::componentName] ? true : false;
            Entity entity = Project::getScene()->createEntity("Entity", isRootEntity, false);
            entity.deserialize(entityYaml);
        }
    }

    std::filesystem::path Project::getPathHelper() {
        return { this->path };
    }

    Scene *Project::getScene() {
        return getInstance().scene;
    }

    Scene &Project::getSceneReference() {
        return *(getInstance().scene);
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

    Dream::AssetImporter *Project::getAssetImporterHelper() {
        return this->assetImporter;
    }

    Dream::AssetImporter *Project::getAssetImporter() {
        return getInstance().getAssetImporterHelper();
    }

    void Project::recognizeResources() {
        getInstance().recognizeResourcesHelper();
    }

    bool Project::isPlaying() {
        return getInstance().playing;
    }

    void Project::setIsPlaying(bool playing) {
        getInstance().playing = playing;
        Input::activatePointerLock(false);
    }

    bool Project::isFullscreen() {
        return getInstance().fullscreen;
    }

    void Project::setIsFullscreen(bool fullscreen) {
        getInstance().fullscreen = fullscreen;
    }

    bool Project::isEditorFullscreen() {
        return getInstance().editorFullscreen;
    }

    void Project::setIsEditorFullscreen(bool editorFullscreen) {
        getInstance().editorFullscreen = editorFullscreen;
    }
}
