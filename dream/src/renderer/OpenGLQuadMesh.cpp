//
// Created by Deepak Ramalingam on 2/27/23.
//

#include "dream/renderer/OpenGLQuadMesh.h"

namespace Dream {
    OpenGLQuadMesh::OpenGLQuadMesh() {
//        glm::vec3 pos1(-1.0,  1.0, 0.0);
//        glm::vec3 pos2(-1.0, -1.0, 0.0);
//        glm::vec3 pos3( 1.0, -1.0, 0.0);
//        glm::vec3 pos4( 1.0,  1.0, 0.0);
        glm::vec3 pos1(-1.0,  1.0 + 1.0, 0.0);
        glm::vec3 pos2(-1.0, -1.0 + 1.0, 0.0);
        glm::vec3 pos3( 1.0, -1.0 + 1.0, 0.0);
        glm::vec3 pos4( 1.0,  1.0 + 1.0, 0.0);

        glm::vec2 uv1(0.0, 1.0);
        glm::vec2 uv2(0.0, 0.0);
        glm::vec2 uv3(1.0, 0.0);
        glm::vec2 uv4(1.0, 1.0);

        glm::vec3 nm(0.0, 0.0, 1.0);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        std::vector<glm::vec3> positions ={
            pos1, pos2, pos3,
            pos1, pos3, pos4
        };

        std::vector<glm::vec2> uv = {
            uv1, uv2, uv3,
            uv1, uv3, uv4
        };

        std::vector<glm::vec3> normals ={
            nm, nm, nm,
            nm, nm, nm
        };

        std::vector<glm::vec3> tangents = {
            tangent1,
            tangent1,
            tangent1,
            tangent2,
            tangent2,
            tangent2
        };

        std::vector<glm::vec3> bitangents = {
                bitangent1,
                bitangent1,
                bitangent1,
                bitangent2,
                bitangent2,
                bitangent2
        };

        for (int i = 0; i < positions.size(); ++i) {
            Vertex vertex = {
                    .position = positions[i],
                    .uv = uv[i],
                    .normal = normals[i],
                    .tangent = tangents[i],
                    .bitangent = bitangents[i]
            };
            for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
                vertex.boneWeights[j] = 0;
                vertex.boneIDs[j] = -1;
            }
            vertices.push_back(vertex);
        }
        this->finalize();
    }
}