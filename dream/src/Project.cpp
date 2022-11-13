//
// Created by Deepak Ramalingam on 10/29/22.
//

#include "dream/Project.h"

#include <utility>

namespace Dream {
    Project::Project() = default;

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
}
