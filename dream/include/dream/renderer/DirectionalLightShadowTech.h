//
// Created by Deepak Ramalingam on 2/10/23.
//

#ifndef DREAM_DIRECTIONALLIGHTSHADOWTECH_H
#define DREAM_DIRECTIONALLIGHTSHADOWTECH_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "dream/renderer/Camera.h"

namespace Dream {
    class DirectionalLightShadowTech {
    public:
        DirectionalLightShadowTech();

        std::vector<glm::mat4> getLightSpaceMatrices(Camera camera, glm::vec3 lightDir);

        std::vector<float> getShadowCascadeLevels(Camera camera);

        glm::vec3 getDirectionalLightDirection();

        int getNumCascades();

    private:
        std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &projview);

        std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view);

        glm::mat4 getLightSpaceMatrix(Camera camera, glm::vec3 lightDir, const float nearPlane, const float farPlane);

        std::vector<glm::mat4> lightSpaceMatrices;

        int numCascades;
    };
}

#endif //DREAM_DIRECTIONALLIGHTSHADOWTECH_H
