//
// Created by Deepak Ramalingam on 10/29/22.
//

#ifndef DREAM_PROJECT_H
#define DREAM_PROJECT_H

#include <iostream>
#include <filesystem>
#include "dream/scene/Scene.h"

namespace Dream {
    class Project {
    public:
        static Project& getInstance()
        {
            static Project instance;
            return instance;
        }
    public:
        Project(Project const&) = delete;
        void operator=(Project const&) = delete;
        static void open(std::filesystem::path filepath);
        static std::filesystem::path getPath();
        Scene &getScene();
    private:
        Project();
        std::filesystem::path path;
        void openHelper(std::filesystem::path filepath);
        std::filesystem::path getPathHelper();
        Scene scene;
    };
}

#endif //DREAM_PROJECT_H
