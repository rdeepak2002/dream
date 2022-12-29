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
        static Project &getInstance() {
            static Project instance;
            return instance;
        }

    public:
        Project(Project const &) = delete;

        ~Project();

        void operator=(Project const &) = delete;

        static void open(std::filesystem::path filepath);

        static std::filesystem::path getPath();

        static Dream::AssetLoader *getAssetLoader();

        static Dream::ResourceManager *getResourceManager();

        static Dream::AssetImporter *getAssetImporter();

        static Dream::Scene *getScene();

        static Dream::Scene &getSceneReference();

        static void recognizeResources();

        static void saveScene();

        static void reloadScene();

        static bool isPlaying();

        static void setIsPlaying(bool playing);

        static bool isFullscreen();

        static void setIsFullscreen(bool fullscreen);

        static bool isEditorFullscreen();

        static void setIsEditorFullscreen(bool editorFullscreen);

    private:
        Project();

        void recognizeResourcesHelper();

        void openHelper(std::filesystem::path filepath);

        void loadScene();

        Dream::AssetLoader *getAssetLoaderHelper();

        Dream::ResourceManager *getResourceManagerHelper();

        Dream::AssetImporter *getAssetImporterHelper();

        std::filesystem::path getPathHelper();

        Scene *getSceneHelper();

        Scene *scene;
        std::filesystem::path path;
        Dream::AssetLoader *assetLoader;
        Dream::ResourceManager *resourceManager;
        Dream::AssetImporter *assetImporter;
        bool playing;
        bool fullscreen;
        bool editorFullscreen;
    };
}

#endif //DREAM_PROJECT_H
