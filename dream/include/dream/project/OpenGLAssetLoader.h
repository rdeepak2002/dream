//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_OPENGLMESHIMPORTER_H
#define DREAM_OPENGLMESHIMPORTER_H

#include "dream/project/AssetLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Dream {
    class OpenGLAssetLoader : public AssetLoader {
    public:
        Entity loadMesh(std::string path) override;
    private:
        Entity processNode(std::string path, aiNode *node, const aiScene *scene, int nodeID);
        Entity processMesh(std::string path, aiMesh *mesh, const aiScene *scene, int meshID);
    };
}

#endif //DREAM_OPENGLMESHIMPORTER_H
