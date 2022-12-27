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

#ifndef DREAM_MESH_H
#define DREAM_MESH_H

#include <vector>
#include <functional>
#include <glm/glm.hpp>

#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

namespace Dream {
    struct Vertex {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        // bones that influence this vertex
        int boneIDs[MAX_BONE_INFLUENCE];
        // effect of each bone on this vertex
        float boneWeights[MAX_BONE_INFLUENCE];
    };

    class Mesh {
    public:
        void setIndices(std::vector<unsigned int> indices);

        void setVertices(std::vector<Vertex> vertices);

        virtual std::vector<unsigned int> getIndices();

        virtual std::vector<Vertex> getVertices();

    protected:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };
}

#endif //DREAM_MESH_H
