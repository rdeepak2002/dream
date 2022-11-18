//
// Created by Deepak Ramalingam on 10/29/22.
//

#ifndef DREAM_PROJECT_H
#define DREAM_PROJECT_H

#include <iostream>
#include <filesystem>
#include "dream/scene/Scene.h"
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
        static Dream::AssetImporter* getAssetImporter();
        static Dream::ResourceManager* getResourceManager();
        static Scene &getScene();
    private:
        Project();
        std::filesystem::path path;
        void openHelper(std::filesystem::path filepath);
        Dream::AssetImporter* getAssetImporterHelper();
        Dream::ResourceManager* getResourceManagerHelper();
        std::filesystem::path getPathHelper();
        Scene &getSceneHelper();
        Scene scene;
        Dream::AssetImporter* assetImporter;
        Dream::ResourceManager* resourceManager;
    };
}

#endif //DREAM_PROJECT_H
