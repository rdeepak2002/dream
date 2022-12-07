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
        Entity loadMesh(std::string guid, bool createEntities = false, Entity rootEntity= {}) override;
    private:
        Entity processNode(std::string path, std::string guid, aiNode *node, const aiScene *scene, bool createEntities, Entity rootEntity);
        Entity processMesh(std::string path, std::string guid, aiMesh *mesh, const aiScene *scene, bool createEntities);
        void setVertexBoneData(Vertex& vertex, int boneID, float weight);
        bool removeNonArmatureEntity(Entity entity);
        static glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from);
        /**
         * Assign a unique ID per submesh within a single Mesh (allows for unique FileID's)
         */
        int meshID = 0;
        int boneCount = 0;
        std::vector<Entity> boneEntities;
        std::map<std::string, Entity> nodeEntities;
    };
}

#endif //DREAM_OPENGLMESHIMPORTER_H
