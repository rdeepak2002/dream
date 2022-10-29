//
// Created by Deepak Ramalingam on 10/29/22.
//

#ifndef DREAM_PROJECT_H
#define DREAM_PROJECT_H

#include <iostream>
#include <filesystem>

class Project {
public:
    explicit Project(std::filesystem::path filepath);
    std::filesystem::path getPath();
private:
    std::filesystem::path filepath;
};


#endif //DREAM_PROJECT_H
