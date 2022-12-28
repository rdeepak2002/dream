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

#include "dream/util/YAMLUtils.h"

namespace YAML {
    Node convert<glm::vec2>::encode(const glm::vec2 &rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    bool convert<glm::vec2>::decode(const Node &node, glm::vec2 &rhs) {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }

    Node convert<glm::vec3>::encode(const glm::vec3 &rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    bool convert<glm::vec3>::decode(const Node &node, glm::vec3 &rhs) {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }

    Node convert<glm::vec4>::encode(const glm::vec4 &rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    bool convert<glm::vec4>::decode(const Node &node, glm::vec4 &rhs) {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }

    Node convert<glm::quat>::encode(const glm::quat &rhs) {
        Node node;
        node.push_back(rhs.w);
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    bool convert<glm::quat>::decode(const Node &node, glm::quat &rhs) {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.w = node[0].as<float>();
        rhs.x = node[1].as<float>();
        rhs.y = node[2].as<float>();
        rhs.z = node[3].as<float>();
        return true;
    }

    Node convert<glm::mat4>::encode(const glm::mat4 &rhs) {
        Node node;

        for (int i = 0; i < 4; ++i) {
            node.push_back(rhs[i].w);
            node.push_back(rhs[i].x);
            node.push_back(rhs[i].y);
            node.push_back(rhs[i].z);
        }

        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    bool convert<glm::mat4>::decode(const Node &node, glm::mat4 &rhs) {
        if (!node.IsSequence() || node.size() != 16)
            return false;

        int count = 0;
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                rhs[r][c] = node[count].as<float>();
                count++;
            }
        }

        return true;
    }
}
