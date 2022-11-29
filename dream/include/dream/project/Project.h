//
// Created by Deepak Ramalingam on 10/29/22.
//

#ifndef DREAM_PROJECT_H
#define DREAM_PROJECT_H

#include <iostream>
#include <filesystem>
#include "dream/scene/Scene.h"
#include "dream/project/AssetLoader.h"
#include "dream/project/AssetImporter.h"
#include "dream/project/ResourceManager.h"

namespace Dream {
    class Project {
    private:
        static Project& getInstance()
        {
            static Project instance;
            return instance;
        }
    public:
        Project(Project const&) = delete;
        ~Project();
        void operator=(Project const&) = delete;
        static void open(std::filesystem::path filepath);
        static std::filesystem::path getPath();
        static Dream::AssetLoader* getAssetLoader();
        static Dream::ResourceManager* getResourceManager();
        static Dream::AssetImporter* getAssetImporter();
        static Dream::Scene* getScene();
        static void recognizeResources();
        static void saveScene();
    private:
        Project();
        void recognizeResourcesHelper();
        void openHelper(std::filesystem::path filepath);
        void loadScene();
        Dream::AssetLoader* getAssetLoaderHelper();
        Dream::ResourceManager* getResourceManagerHelper();
        Dream::AssetImporter* getAssetImporterHelper();
        std::filesystem::path getPathHelper();
        Scene *getSceneHelper();
        Scene *scene;
        std::filesystem::path path;
        Dream::AssetLoader* assetLoader;
        Dream::ResourceManager* resourceManager;
        Dream::AssetImporter* assetImporter;
    };
}

#endif //DREAM_PROJECT_H
