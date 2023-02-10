//
// Created by Deepak Ramalingam on 2/10/23.
//

#include "dream/renderer/DirectionalLightShadowTech.h"
#include "dream/project/Project.h"
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    DirectionalLightShadowTech::DirectionalLightShadowTech() {
        this->numCascades = 4;
    }

    std::vector<glm::vec4> DirectionalLightShadowTech::getFrustumCornersWorldSpace(const glm::mat4 &projview) {
        const auto inv = glm::inverse(projview);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    std::vector<glm::vec4>
    DirectionalLightShadowTech::getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view) {
        return getFrustumCornersWorldSpace(proj * view);
    }

    glm::mat4 DirectionalLightShadowTech::getLightSpaceMatrix(Camera camera, glm::vec3 lightDir, const float nearPlane,
                                                              const float farPlane) {
        const auto proj = glm::perspective(glm::radians(camera.fov), camera.getAspect(), nearPlane, farPlane);
        const auto corners = getFrustumCornersWorldSpace(proj, camera.getViewMatrix());

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto &v: corners) {
            center += glm::vec3(v);
        }
        center /= corners.size();

        const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto &v: corners) {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0) {
            minZ *= zMult;
        } else {
            minZ /= zMult;
        }

        if (maxZ < 0) {
            maxZ /= zMult;
        } else {
            maxZ *= zMult;
        }

        const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

        return lightProjection * lightView;
    }

    std::vector<glm::mat4> DirectionalLightShadowTech::getLightSpaceMatrices(Camera camera, glm::vec3 lightDir) {
        auto shadowCascadeLevels = getShadowCascadeLevels(camera);
        std::vector<glm::mat4> ret;
        for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i) {
            if (i == 0) {
                ret.push_back(getLightSpaceMatrix(camera, lightDir, camera.zNear, shadowCascadeLevels[i]));
            } else if (i < shadowCascadeLevels.size()) {
                ret.push_back(
                        getLightSpaceMatrix(camera, lightDir, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
            } else {
                ret.push_back(getLightSpaceMatrix(camera, lightDir, shadowCascadeLevels[i - 1], camera.zNear));
            }
        }
        return ret;
    }

    std::vector<float> DirectionalLightShadowTech::getShadowCascadeLevels(Camera camera) {
        float cameraFarPlane = camera.zFar;
        std::vector<float> shadowCascadeLevels{8.0f, 10.0f * 2, 36.0f * 2, 500.0f * 2};
//        std::vector<float> shadowCascadeLevels{ cameraFarPlane / 26.0f, cameraFarPlane / 15.0f, cameraFarPlane / 7.0f, cameraFarPlane - 0.001f };
//        std::vector<float> shadowCascadeLevels{ cameraFarPlane / 26.0f, cameraFarPlane / 15.0f, cameraFarPlane / 7.0f, cameraFarPlane / 2.0f };
        if (numCascades != shadowCascadeLevels.size()) {
            Logger::fatal("Number of cascades does not match number of shadow levels specified");
        }
        return shadowCascadeLevels;
    }

    glm::vec3 DirectionalLightShadowTech::getDirectionalLightDirection() {
        glm::vec3 lightDir(1, 0, 0);
        auto lightEntities = Project::getScene()->getEntitiesWithComponents<Component::LightComponent>();
        for (const auto &lightEntity : lightEntities) {
            Entity entity = {lightEntity, Project::getScene()};
            if (entity.getComponent<Component::LightComponent>().type == Component::LightComponent::LightType::DIRECTIONAL) {
                lightDir = glm::normalize(entity.getComponent<Component::TransformComponent>().getFront());
                lightDir *= -1;
            }
        }
        return lightDir;
    }

    int DirectionalLightShadowTech::getNumCascades() {
        return numCascades;
    }
}