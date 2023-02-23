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

#include "dream/scene/system/TerrainComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"

namespace Dream {
    void gaussianFilter(double GKernel[][5]) {
        // initialising standard deviation to 1.0
        double sigma = 1.0;
        double r, s = 2.0 * sigma * sigma;

        // sum is for normalization
        double sum = 0.0;

        // generating 5x5 kernel
        for (int x = -2; x <= 2; x++) {
            for (int y = -2; y <= 2; y++) {
                r = sqrt(x * x + y * y);
                GKernel[x + 2][y + 2] = (exp(-(r * r) / s)) / (M_PI * s);
                sum += GKernel[x + 2][y + 2];
            }
        }

        // normalising the Kernel
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                GKernel[i][j] /= sum;
    }

    std::optional<glm::vec3> intersectRayWithPlane(
            glm::vec3 p, glm::vec3 v,  // ray
            glm::vec3 n, float d  // plane
    ) {
        float denom = dot(n, v);

        // Prevent divide by zero:
        if (abs(denom) <= 1e-4f)
            return std::nullopt;

        // If you want to ensure the ray reflects off only
        // the "top" half of the plane, use this instead:
        //
        // if (-denom <= 1e-4f)
        //     return std::nullopt;

        float t = -(dot(n, p) + d) / dot(n, v);

        // Use pointy end of the ray.
        // It is technically correct to compare t < 0,
        // but that may be undesirable in a raytracer.
        if (t <= 1e-4)
            return std::nullopt;

        return p + t * v;
    }

    void TerrainComponentSystem::init() {

    }

    void TerrainComponentSystem::update(float dt) {
        auto sceneCameraEntity = Project::getScene()->getSceneCamera();
        if (!sceneCameraEntity) {
            return;
        }
        // update all entities with terrain components
        auto rigidBodyEntities = Project::getScene()->getEntitiesWithComponents<Component::TerrainComponent>();
        for (auto entityHandle: rigidBodyEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (!Project::isPlaying() && Input::getButtonDown(Key::LeftMouse)) {
                // TODO: make renderer agnostic
                OpenGLBaseTerrain *terrain = entity.getComponent<Component::TerrainComponent>().terrain;
                if (terrain) {
                    int viewportWidth = Input::getRendererDimensions().first;
                    int viewportHeight = Input::getRendererDimensions().second;

                    Camera camera = {(float) viewportWidth * 2.0f, (float) viewportHeight * 2.0f};
                    sceneCameraEntity.getComponent<Component::SceneCameraComponent>().updateRendererCamera(camera, sceneCameraEntity);

                    float mouseX = Input::getRelativeMousePosition().x;
                    float mouseY = Input::getRelativeMousePosition().y;

                    glm::mat4 proj = camera.getProjectionMatrix();
                    glm::mat4 view = camera.getViewMatrix();

                    glm::mat4 invVP = glm::inverse(proj * view);
                    glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
                    glm::vec4 worldPos = invVP * screenPos;

                    glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

                    // TODO: use raycasting to get x, z position of terrain to modify
//                    auto sceneCameraFront = sceneCameraEntity.getComponent<Component::SceneCameraComponent>().front;
                    auto sceneCameraCenter = sceneCameraEntity.getComponent<Component::TransformComponent>().translation;

                    float d = 0;
                    auto maybeIntersectionPoint = intersectRayWithPlane(sceneCameraCenter, dir,glm::vec3(0, 1, 0), d);

                    if (maybeIntersectionPoint) {
                        glm::vec3 intersectionPoint = *maybeIntersectionPoint;

                        float xCoordOnPlane = intersectionPoint.x / terrain->getWorldScale();
                        float zCoordOnPlane = intersectionPoint.z / terrain->getWorldScale();

                        // get width and height, then project
                        int x = (int) xCoordOnPlane;
                        int z = (int) zCoordOnPlane;

                        int startX = x - 5 / 2;
                        int startZ = z - 5 / 2;

                        double GKernel[5][5];
                        gaussianFilter(GKernel);
                        for (int i = 0; i < 5; ++i) {
                            for (int j = 0; j < 5; ++j) {
                                float deltaHeight = 200.0f;
                                float currentHeight = terrain->getHeight(x, z);
                                float newHeight = currentHeight + deltaHeight * dt * GKernel[i][j];
                                terrain->setHeight(startX + i, startZ + j, newHeight);
//                                    std::cout << GKernel[i][j] * deltaHeight << "\t";
                            }
//                                std::cout << std::endl;
                        }

                        terrain->refreshTerrainTriangleList();
                    }
                }
            }
        }
    }
}