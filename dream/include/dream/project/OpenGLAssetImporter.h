//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_OPENGLMESHIMPORTER_H
#define DREAM_OPENGLMESHIMPORTER_H

#include "dream/project/AssetImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Dream {
    class OpenGLAssetImporter : public AssetImporter {
    public:
        Entity importMesh(std::string path) override;
    private:
        Entity processNode(std::string path, aiNode *node, const aiScene *scene);
        Entity processMesh(std::string path, aiMesh *mesh, const aiScene *scene);
    };
}

#endif //DREAM_OPENGLMESHIMPORTER_H
