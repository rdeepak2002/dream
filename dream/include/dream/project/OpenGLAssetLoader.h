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
        Entity loadMesh(std::string guid) override;
    private:
        Entity processNode(std::string path, std::string guid, aiNode *node, const aiScene *scene);
        Entity processMesh(std::string path, std::string guid, aiMesh *mesh, const aiScene *scene);
        /**
         * Assign a unique ID per submesh within a single Mesh (allows for unique FileID's)
         */
        int meshID = 0;
    };
}

#endif //DREAM_OPENGLMESHIMPORTER_H
