//
// Created by Deepak Ramalingam on 10/29/22.
//

#include "dream/Project.h"

#include <utility>

Project::Project(std::filesystem::path filepath) {
    if (std::filesystem::exists(filepath)) {
        this->filepath = std::move(filepath);
    } else {
        fprintf(stderr, "Error: project folder does not exist\n");
        exit(EXIT_FAILURE);
    }
}

std::filesystem::path Project::getPath() {
    return this->filepath;
}
