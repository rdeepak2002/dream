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

#include "dream/renderer/OpenGLRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include "dream/project/Project.h"
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/util/Logger.h"
#include "dream/window/Input.h"
#include "dream/renderer/OpenGLCubeMesh.h"

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader programs
        lightingShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                               Project::getPath().append("assets").append("shaders").append(
                                                       "lighting_shader.frag").c_str(), nullptr);

        singleTextureShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                               Project::getPath().append("assets").append("shaders").append(
                                          "shader_single_texture.frag").c_str(), nullptr);

        physicsDebugShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("physics.vert").c_str(),
                                  Project::getPath().append("assets").append("shaders").append(
                                          "physics.frag").c_str(), nullptr);

        skyboxShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("skybox.vert").c_str(),
                                              Project::getPath().append("assets").append("shaders").append(
                                                      "skybox.frag").c_str(), nullptr);

        simpleDepthShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shadow_mapping_depth.vert").c_str(),
                                        Project::getPath().append("assets").append("shaders").append(
                                                "shadow_mapping_depth.frag").c_str(), nullptr);

        terrainShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("terrain_shader.vert").c_str(),
                                             Project::getPath().append("assets").append("shaders").append(
                                                     "terrain_shader.frag").c_str(), nullptr);

        skybox = new OpenGLSkybox();

        outputRenderTextureFbo = new OpenGLFrameBuffer();

        directionalLightShadowTech = new DirectionalLightShadowTech();

        skinningTech = new SkinningTech();

        lightingTech = new LightingTech();

        // TODO: maybe encapsulate this in directional light shadow tech?
        for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
            const unsigned int SHADOW_WIDTH = 1024 * 8, SHADOW_HEIGHT = 1024 * 8;
            auto shadowMapFbo = new OpenGLShadowMapFBO((int) SHADOW_WIDTH, (int) SHADOW_HEIGHT);
            shadowMapFbos.push_back(shadowMapFbo);
        }

        // load primitive shapes
        if (!Project::getResourceManager()->hasMeshData("sphere")) {
            Project::getResourceManager()->storeMeshData(new OpenGLSphereMesh(), "sphere");
        }

        if (!Project::getResourceManager()->hasMeshData("cube")) {
            Project::getResourceManager()->storeMeshData(new OpenGLCubeMesh(), "cube");
        }
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete this->lightingShader;
        delete this->singleTextureShader;
        delete this->physicsDebugShader;
        delete this->simpleDepthShader;
        delete this->outputRenderTextureFbo;
        for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
            delete shadowMapFbos.at(i);
        }
        delete this->lightingTech;
        delete this->directionalLightShadowTech;
        delete this->skinningTech;
        delete this->terrainShader;
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // update renderer camera using scene camera entity's position and camera attributes like yaw, pitch, and fov
        std::optional<Camera> maybeCamera;
        auto sceneCameraEntity = Project::getScene()->getSceneCamera();
        auto mainCameraEntity = Project::getScene()->getMainCamera();

        // update viewport width and height (so relative mouse position on a [-1, 1] range can be calculated)
        Input::getInstance().setRendererDimensions(viewportWidth, viewportHeight);

        // TODO: make camera use global position of entity for camera position
        if (sceneCameraEntity && !Project::isPlaying()) {
            maybeCamera = {(float) viewportWidth * 2.0f, (float) viewportHeight * 2.0f};
            sceneCameraEntity.getComponent<Component::SceneCameraComponent>().updateRendererCamera(*maybeCamera, sceneCameraEntity);
        } else if (mainCameraEntity && Project::isPlaying()) {
            maybeCamera = {(float) viewportWidth * 2.0f, (float) viewportHeight * 2.0f};
            mainCameraEntity.getComponent<Component::CameraComponent>().updateRendererCamera(*maybeCamera, mainCameraEntity);
        }

        if (maybeCamera) {
            auto camera = *maybeCamera;

            // light spaces matrices for shadow cascades
            auto lightSpaceMatrices = directionalLightShadowTech->getLightSpaceMatrices(camera, directionalLightShadowTech->getDirectionalLightDirection());

            {
                // render scene from light's point of view
                glEnable(GL_DEPTH_CLAMP);
                for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
                    simpleDepthShader->use();
                    simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrices.at(i));
                    glViewport(0, 0, (int) shadowMapFbos.at(i)->getWidth(), (int) shadowMapFbos.at(i)->getHeight());
                    shadowMapFbos.at(i)->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                    drawTerrains(camera, simpleDepthShader);
                    drawEntities(Project::getScene()->getRootEntity(), camera, simpleDepthShader);
                    shadowMapFbos.at(i)->unbind();
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                }
                glDisable(GL_DEPTH_CLAMP);
            }

            {
                // bind frame buffer for drawing to output render texture
                glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                this->outputRenderTextureFbo->bindForWriting();
                this->resizeFrameBuffer();
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            {
                // draw terrains
                terrainShader->use();
                terrainShader->setMat4("projection", camera.getProjectionMatrix());
                terrainShader->setMat4("view", camera.getViewMatrix());
                terrainShader->setFloat("farPlane", camera.zFar);
                glm::vec3 viewPos = camera.position;
                terrainShader->setVec3("viewPos", viewPos);
                terrainShader->setVec3("shadowDirectionalLightDir", directionalLightShadowTech->getDirectionalLightDirection());
                for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
                    terrainShader->setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrices.at(i));
                }
                auto shadowCascadeLevels = directionalLightShadowTech->getShadowCascadeLevels(camera);
                for (int i = 0; i < shadowCascadeLevels.size(); ++i) {
                    terrainShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels.at(i));
                }
                drawTerrains(camera, terrainShader);
            }

            {
                // draw meshes
                if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                    lightingShader->use();
                    lightingShader->setMat4("projection", camera.getProjectionMatrix());
                    lightingShader->setMat4("view", camera.getViewMatrix());
                    lightingShader->setFloat("farPlane", camera.zFar);
                    glm::vec3 viewPos = camera.position;
                    lightingShader->setVec3("viewPos", viewPos);
                    lightingShader->setVec3("shadowDirectionalLightDir", directionalLightShadowTech->getDirectionalLightDirection());
                    for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
                        lightingShader->setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrices.at(i));
                    }
                    auto shadowCascadeLevels = directionalLightShadowTech->getShadowCascadeLevels(camera);
                    for (int i = 0; i < shadowCascadeLevels.size(); ++i) {
                        lightingShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels.at(i));
                    }
                    drawEntities(Project::getScene()->getRootEntity(), camera, lightingShader);
                } else {
                    singleTextureShader->use();
                    singleTextureShader->setMat4("projection", camera.getProjectionMatrix());
                    singleTextureShader->setMat4("view", camera.getViewMatrix());
                    drawEntities(Project::getScene()->getRootEntity(), camera, singleTextureShader);
                }
            }

            {
                // draw physics debug
                if (Project::getConfig().physicsConfig.physicsDebugger && (!Project::isPlaying() || (Project::isPlaying() && Project::getConfig().physicsConfig.physicsDebuggerWhilePlaying))) {
                    if (Project::getConfig().physicsConfig.depthTest) {
                        glEnable(GL_DEPTH_TEST);
                    } else {
                        glDisable(GL_DEPTH_TEST);
                    }
                    physicsDebugShader->use();
                    physicsDebugShader->setMat4("projection", camera.getProjectionMatrix());
                    physicsDebugShader->setMat4("view", camera.getViewMatrix());
                    if (Project::getScene()->getPhysicsComponentSystem()) {
                        Project::getScene()->getPhysicsComponentSystem()->debugDrawWorld();
                    }
                    if (!Project::getConfig().physicsConfig.depthTest) {
                        glEnable(GL_DEPTH_TEST);
                    }
                }
            }

            // draw skybox
            {
                // draw skybox as last
                glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
                skyboxShader->use();
                auto skyboxView = glm::mat4(glm::mat3(camera.getViewMatrix()));
                skyboxShader->setMat4("view", skyboxView);
                skyboxShader->setMat4("projection", camera.getProjectionMatrix());
                skyboxShader->setInt("skybox", 0);
                // skybox cube
                glBindVertexArray(skybox->getVAO());
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTexture());
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                glDepthFunc(GL_LESS); // set depth function back to default
            }
        } else {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        // bind the default screen frame buffer
        this->outputRenderTextureFbo->unbind();

        // clear framebuffer and return its texture
        this->outputRenderTextureFbo->clear();

        if (fullscreen) {
            this->outputRenderTextureFbo->renderScreenQuad();
        }
    }

    void OpenGLRenderer::drawTerrains(Camera camera, OpenGLShader* shader) {
        auto terrainEntities = Project::getScene()->getEntitiesWithComponents<Component::TerrainComponent>();
        for (auto entityHandle: terrainEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            // render terrain
            if (!entity.getComponent<Component::TerrainComponent>().guid.empty()) {
                if (!entity.getComponent<Component::TerrainComponent>().terrain) {
                    // TODO: store in resource manager instead
                    // load terrain if necessary
                    entity.getComponent<Component::TerrainComponent>().terrain = new OpenGLBaseTerrain(4.0, 200.0);
                    auto terrainFilePath = Project::getResourceManager()->getFilePathFromGUID(entity.getComponent<Component::TerrainComponent>().guid);
                    entity.getComponent<Component::TerrainComponent>().terrain->loadFromFile(terrainFilePath.c_str());
                }
                for (int i = 0; i < MAX_BONES; i++) {
                    shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.0));
                }
                glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
                shader->setMat4("model", model);
                lightingTech->setTextureAndColorUniforms(entity, shadowMapFbos, directionalLightShadowTech, shader);
                entity.getComponent<Component::TerrainComponent>().terrain->setShaderUniforms(shader);
                entity.getComponent<Component::TerrainComponent>().terrain->render();
            }
        }
    }

    void OpenGLRenderer::drawEntities(Entity entity, Camera camera, OpenGLShader* shader) {
        // set bones for animated meshes
        skinningTech->setJointUniforms(entity, shader);

        // draw mesh of entity
        if (entity.hasComponent<Component::MeshComponent>()) {
            // load mesh of entity
            // TODO: only load when necessary (add a flag internally - and reset it when fields are modified)
            entity.getComponent<Component::MeshComponent>().loadMesh();

            // set textures and colors used for the shader code to color, light, and shade the entity
            lightingTech->setTextureAndColorUniforms(entity, shadowMapFbos, directionalLightShadowTech, shader);

            // get transform of entity
            glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
            shader->setMat4("model", model);

            // draw mesh of entity
            if (!entity.getComponent<Component::MeshComponent>().fileId.empty() || entity.getComponent<Component::MeshComponent>().meshType != Component::MeshComponent::FROM_FILE) {
                auto guid = entity.getComponent<Component::MeshComponent>().getMeshGuid();
                auto fileId = entity.getComponent<Component::MeshComponent>().getMeshFileID();
                auto mesh = Project::getResourceManager()->getMeshData(guid, fileId);
                if (auto openGLMesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh)) {
                    drawMesh(openGLMesh);
                } else {
                    Logger::fatal("Unable to dynamic cast Mesh to type OpenGLMesh for entity " + entity.getComponent<Component::IDComponent>().id);
                }
            }
        }

        // draw child entities
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            drawEntities(child, camera, shader);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }

    void OpenGLRenderer::drawMesh(std::shared_ptr<OpenGLMesh> openGLMesh) {
        if (!openGLMesh->getIndices().empty()) {
            // case where vertices are indexed
            auto numIndices = openGLMesh->getIndices().size();
            glBindVertexArray(openGLMesh->getVAO());
            glDrawElements(GL_TRIANGLES, (int) numIndices, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        } else if (!openGLMesh->getVertices().empty()) {
            // case where vertices are not indexed
            glBindVertexArray(openGLMesh->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, (int) openGLMesh->getVertices().size());
            glBindVertexArray(0);
        } else {
            Logger::fatal("Unable to render mesh");
        }
    }

    void OpenGLRenderer::resizeFrameBuffer() {
        // resize framebuffer whenever there is a new viewport size
        GLint viewportWidth = this->getViewportDimensions().first;
        GLint viewportHeight = this->getViewportDimensions().second;
        if (outputRenderTextureFbo->getWidth() != viewportWidth || outputRenderTextureFbo->getHeight() != viewportHeight) {
            outputRenderTextureFbo->resize(viewportWidth, viewportHeight);
        }
    }

    void OpenGLRenderer::printGLVersion() {
        Logger::info("GL Vendor: " + std::string((const char *) glGetString(GL_VENDOR)));
        Logger::info("GL Renderer: " + std::string((const char *) glGetString(GL_RENDERER)));
        Logger::info("GL Version: " + std::string((const char *) glGetString(GL_VERSION)));
    }

    std::pair<int, int> OpenGLRenderer::getViewportDimensions() {
        GLint dims[4] = {0};
        glGetIntegerv(GL_VIEWPORT, dims);
        return std::make_pair(dims[2], dims[3]);
    }

    unsigned int OpenGLRenderer::getOutputRenderTexture() {
//        return this->shadowMapFbos.at(0)->getTexture();
        return this->outputRenderTextureFbo->getTexture();
    }
}
