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
#include "dream/renderer/OpenGLQuadMesh.h"
#include "dream/util/MathUtils.h"

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader programs
        lightingShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                               Project::getPath().append("assets").append("shaders").append(
                                                       "lighting_shader.frag").c_str(), nullptr);

        instancedLightingShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader_instanced.vert").c_str(),
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

        hdrFrameBuffer = new OpenGLFrameBuffer();

        directionalLightShadowTech = new DirectionalLightShadowTech();

        skinningTech = new SkinningTech();

        lightingTech = new LightingTech();

        // TODO: maybe encapsulate this in directional light shadow tech?
        for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
            int scale = 1;
            if (i == 0) {
                scale = 8;
            } else if (i == 1) {
                scale = 4;
            } else {
                scale = 2;
            }

            const unsigned int SHADOW_WIDTH = 1024 * scale, SHADOW_HEIGHT = 1024 * scale;
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

        if (!Project::getResourceManager()->hasMeshData("quad")) {
            Project::getResourceManager()->storeMeshData(new OpenGLQuadMesh(), "quad");
        }
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete this->lightingShader;
        delete this->instancedLightingShader;
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
        delete this->hdrFrameBuffer;
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_MULTISAMPLE);

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
#ifndef EMSCRIPTEN
                glEnable(GL_DEPTH_CLAMP);
#endif
                for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
                    simpleDepthShader->use();
                    simpleDepthShader->setBool("instanced", false);
                    simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrices.at(i));
                    glViewport(0, 0, (int) shadowMapFbos.at(i)->getWidth(), (int) shadowMapFbos.at(i)->getHeight());
                    shadowMapFbos.at(i)->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                    drawTerrains(camera, simpleDepthShader);
                    drawEntities(Project::getScene()->getRootEntity(), camera, simpleDepthShader);
                    simpleDepthShader->setBool("instanced", true);
                    drawInstancedMeshes(camera, simpleDepthShader);
                    shadowMapFbos.at(i)->unbind();
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                }
#ifndef EMSCRIPTEN
                glDisable(GL_DEPTH_CLAMP);
#endif
            }

            {
                // bind frame buffer for drawing to output render texture
                glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                this->outputRenderTextureFbo->bindForWriting();
                {
                    // resize framebuffer whenever there is a new viewport size
                    GLint viewportWidth = this->getViewportDimensions().first;
                    GLint viewportHeight = this->getViewportDimensions().second;
                    if (outputRenderTextureFbo->getWidth() != viewportWidth || outputRenderTextureFbo->getHeight() != viewportHeight) {
                        outputRenderTextureFbo->resize(viewportWidth, viewportHeight);
                    }
                }
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            {
                // draw meshes
                if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                    lightingShader->use();
                    lightingTech->applyFog(lightingShader);
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
                // draw instanced meshes
                instancedLightingShader->use();
                lightingTech->applyFog(instancedLightingShader);
                instancedLightingShader->setMat4("projection", camera.getProjectionMatrix());
                instancedLightingShader->setMat4("view", camera.getViewMatrix());
                instancedLightingShader->setFloat("farPlane", camera.zFar);
                glm::vec3 viewPos = camera.position;
                instancedLightingShader->setVec3("viewPos", viewPos);
                instancedLightingShader->setVec3("shadowDirectionalLightDir", directionalLightShadowTech->getDirectionalLightDirection());
                for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
                    instancedLightingShader->setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrices.at(i));
                }
                auto shadowCascadeLevels = directionalLightShadowTech->getShadowCascadeLevels(camera);
                for (int i = 0; i < shadowCascadeLevels.size(); ++i) {
                    instancedLightingShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels.at(i));
                }
                drawInstancedMeshes(camera, instancedLightingShader);
            }

            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                // draw terrains
                terrainShader->use();
                lightingTech->applyFog(terrainShader);
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
                glDisable(GL_CULL_FACE);
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

        {
            // bind HDR frame buffer
            glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            this->hdrFrameBuffer->bindForWriting();
            {
                // resize framebuffer whenever there is a new viewport size
                GLint viewportWidth = this->getViewportDimensions().first;
                GLint viewportHeight = this->getViewportDimensions().second;
                if (hdrFrameBuffer->getWidth() != viewportWidth || hdrFrameBuffer->getHeight() != viewportHeight) {
                    hdrFrameBuffer->resize(viewportWidth, viewportHeight);
                }
            }
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        this->outputRenderTextureFbo->renderScreenQuad();

        // bind the default screen frame buffer
        this->hdrFrameBuffer->unbind();

        // clear framebuffer and return its texture
        this->hdrFrameBuffer->clear();

        if (fullscreen) {
            this->outputRenderTextureFbo->renderScreenQuad();
//            this->hdrFrameBuffer->renderScreenQuad();
        }
    }

    void OpenGLRenderer::getMeshesForModel(Entity entity, std::vector<Entity> &vec) {
        // add primitives
        if (entity.hasComponent<Component::MeshComponent>()) {
            auto meshComponent = entity.getComponent<Component::MeshComponent>();
            if (meshComponent.meshType != Component::MeshComponent::FROM_FILE) {
                vec.push_back(entity);
            }
        }
        // get child meshes for a model and store them in the parameter vector
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            if (child.hasComponent<Component::MeshComponent>()) {
                auto meshComponent = child.getComponent<Component::MeshComponent>();
                if (!meshComponent.guid.empty() && !meshComponent.fileId.empty()) {
                    vec.push_back(child);
                }
                if (meshComponent.guid.empty() || !meshComponent.fileId.empty()) {
                    getMeshesForModel(child, vec);
                }
            } else {
                getMeshesForModel(child, vec);
            }
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }

    void OpenGLRenderer::drawInstancedMeshes(Camera camera, OpenGLShader* shader) {
        // TODO: this is getting a specific instanced model, instead generalize
        std::vector<Entity> modelEntities;
        std::vector<unsigned int> amounts;
        std::vector<glm::vec3> offsets;
        std::vector<float> displacements;
        std::vector<bool> isBillboards;
        std::vector<bool> centerAroundCamera;

        static std::vector<glm::vec3> pointsAcrossTerrain;
        static bool initializePoints = true;
        int maxNumPoints = 250000;
        float grassSpawnRadius = 40.0f;

        if (initializePoints) {
            while (pointsAcrossTerrain.size() < maxNumPoints) {
                // generate points somewhere around camera
                int i = pointsAcrossTerrain.size();
                glm::vec3 p = glm::vec3(MathUtils::randomFloat(-grassSpawnRadius, grassSpawnRadius), 0, MathUtils::randomFloat(-grassSpawnRadius, grassSpawnRadius)) + glm::vec3(camera.position.x, 0, camera.position.z);

                // remove grass near spawn (or other invalid areas)
                while (MathUtils::distance(p, glm::vec3(0, 0, 0)) < 2.0f) {
                    p = glm::vec3(MathUtils::randomFloat(-grassSpawnRadius, grassSpawnRadius), 0, MathUtils::randomFloat(-grassSpawnRadius, grassSpawnRadius)) + glm::vec3(camera.position.x, 0, camera.position.z);
                }

                pointsAcrossTerrain.push_back(p);
            }

            initializePoints = false;
        }

        modelEntities.push_back(Project::getScene()->getEntityByTag("forest tree"));
//        amounts.push_back(600);
        amounts.push_back(300);
        offsets.emplace_back(0, 0, 0);
        displacements.emplace_back(80);
        isBillboards.emplace_back(false);
        centerAroundCamera.emplace_back(false);

        // do not cast shadows for grass
        if (shader != simpleDepthShader) {
            modelEntities.push_back(Project::getScene()->getEntityByTag("grass"));
            amounts.push_back(4000);
            offsets.emplace_back(0, 0.14, 0);
            displacements.emplace_back(10);
            isBillboards.emplace_back(true);
            centerAroundCamera.emplace_back(true);
        }

        for (int j = 0; j < modelEntities.size(); ++j) {
            auto isCenteredAroundCamera = centerAroundCamera.at(j);
            auto amount = isCenteredAroundCamera ? pointsAcrossTerrain.size() : amounts.at(j);
            auto modelEntity = modelEntities.at(j);
            auto offset = offsets.at(j);
            auto displacement = displacements.at(j);
            auto isBillboard = isBillboards.at(j);

            if (amount == 0) {
                continue;
            }

            std::vector<Entity> instancedMeshEntities;
            getMeshesForModel(modelEntity, instancedMeshEntities);
            shader->setBool("billboard", isBillboard);

            for (int i = 0; i < instancedMeshEntities.size(); ++i) {
                Entity entity = instancedMeshEntities.at(i);
                auto modelGUID = entity.getComponent<Component::MeshComponent>().getMeshGuid();
                auto meshFileID = entity.getComponent<Component::MeshComponent>().getMeshFileID();

                static std::unordered_set<std::string> processedMeshEntities;

                if (!processedMeshEntities.count(entity.getID())) {
                    auto* modelMatrices = new glm::mat4[amount];
                    for (unsigned int i = 0; i < amount; i++)
                    {
                        // TODO: have boolean if we want to filter only the closest things to our player (like only draw grass close to player and sample a few from farther ones)
                        // TODO: make sure you also update amount
                        glm::mat4 model = glm::mat4(1.0f);
                        // TODO: it is slow calling srand every loop, instead call this once and store positions in some array
                        srand(3 * i * i + 4 * i + 9);
                        if (isCenteredAroundCamera) {
                            model = glm::translate(model, pointsAcrossTerrain.at(i) + offset);
                        } else {
                            model = glm::translate(model, glm::vec3(MathUtils::randomFloat(-displacement / 2, displacement / 2),  0,  MathUtils::randomFloat(-displacement / 2, displacement / 2)) + offset);
                        }
                        model = glm::scale(model, modelEntity.getComponent<Component::TransformComponent>().scale);
                        model = model * glm::toMat4(modelEntity.getComponent<Component::TransformComponent>().rotation);
                        modelMatrices[i] = model;
                    }

                    // TODO: cache this buffer rather than creating it every frame to improve performance?
                    unsigned int instancingModelMatricesBuffer;
                    glGenBuffers(1, &instancingModelMatricesBuffer);
                    glBindBuffer(GL_ARRAY_BUFFER, instancingModelMatricesBuffer);
                    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

                    auto mesh = Project::getResourceManager()->getMeshData(modelGUID, meshFileID);
                    if (auto openGLMesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh)) {
                        glBindVertexArray(openGLMesh->getVAO());
                    } else {
                        Logger::fatal("Error getting instanced mesh (1)");
                    }

                    // set attribute pointers for matrix (4 times vec4)
                    glEnableVertexAttribArray(7);
                    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
                    glEnableVertexAttribArray(8);
                    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
                    glEnableVertexAttribArray(9);
                    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
                    glEnableVertexAttribArray(10);
                    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

                    glVertexAttribDivisor(7, 1);
                    glVertexAttribDivisor(8, 1);
                    glVertexAttribDivisor(9, 1);
                    glVertexAttribDivisor(10, 1);

                    glBindVertexArray(0);

                    delete[] modelMatrices;
                    glDeleteBuffers(1, &instancingModelMatricesBuffer);

                    processedMeshEntities.insert(entity.getID());
                }

                // TODO: clean up this method and use the instanced meshes
                for (int i = 0; i < MAX_BONES; i++) {
                    shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.0));
                }

                lightingTech->setTextureAndColorUniforms(entity, shadowMapFbos, directionalLightShadowTech, shader);

                // TODO: basically look for all meshes (not model parent) w/ a particular GUID and loop calling this below chunk of code for each unique GUID
                {
                    // run code for all mesh's with GUID a, then run this code for all meshes with GUID b, etc.
                    auto mesh = Project::getResourceManager()->getMeshData(modelGUID, meshFileID);
                    if (auto openGLMesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh)) {
                        glBindVertexArray(openGLMesh->getVAO());
                        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(openGLMesh->getIndices().size()), GL_UNSIGNED_INT, 0, amount);
                    } else {
                        Logger::fatal("Error getting instanced mesh (2)");
                    }
                    glBindVertexArray(0);
                }
            }
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
//                    entity.getComponent<Component::TerrainComponent>().terrain = new OpenGLBaseTerrain(4.0, 200.0);
//                    auto terrainFilePath = Project::getResourceManager()->getFilePathFromGUID(entity.getComponent<Component::TerrainComponent>().guid);
//                    entity.getComponent<Component::TerrainComponent>().terrain->loadFromFile(terrainFilePath.c_str());
                    entity.getComponent<Component::TerrainComponent>().initializeTerrain();
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
//        return this->outputRenderTextureFbo->getTexture();
        return this->hdrFrameBuffer->getTexture();
    }
}
