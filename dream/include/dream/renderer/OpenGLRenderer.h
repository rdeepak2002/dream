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

#ifndef DREAM_OPENGLRENDERER_H
#define DREAM_OPENGLRENDERER_H

#include "dream/renderer/Renderer.h"
#include "dream/renderer/OpenGLShader.h"
#include "dream/renderer/OpenGLFrameBuffer.h"
#include "dream/renderer/OpenGLShadowMapFBO.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLSphereMesh.h"
#include "dream/renderer/Mesh.h"
#include "dream/scene/Entity.h"
#include "dream/renderer/OpenGLSkybox.h"
#include "Camera.h"

namespace Dream {
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();

        ~OpenGLRenderer();

        void render(int viewportWidth, int viewportHeight, bool fullscreen) override;

        unsigned int getOutputRenderTexture() override;

    private:
        OpenGLShader *lightingShader;
        OpenGLShader *singleTextureShader;
        OpenGLShader *physicsDebugShader;
        OpenGLShader *skyboxShader;
        OpenGLShader *simpleDepthShader;
        OpenGLFrameBuffer *frameBuffer;
        std::vector<OpenGLShadowMapFBO *> shadowMapFbos;
        OpenGLTexture *whiteTexture;
        OpenGLTexture *blackTexture;
        OpenGLSkybox *skybox;

        const unsigned int SHADOW_WIDTH = 1024 * 8, SHADOW_HEIGHT = 1024 * 8, NUM_CASCADES = 4;

        void resizeFrameBuffer();

        void printGLVersion();

        void renderEntityAndChildren(Entity entity);

        void applyLighting();

        void renderScene(OpenGLShader* shader);

        void renderSceneHelper(Entity entity, OpenGLShader* shader);

        std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

        std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

        glm::mat4 getLightSpaceMatrix(Camera camera, glm::vec3 lightDir, const float nearPlane, const float farPlane);

        std::vector<glm::mat4> getLightSpaceMatrices(Camera camera, glm::vec3 lightDir);

        std::pair<int, int> getViewportDimensions();

        glm::vec3 getDirectionalLightDirection();

        std::vector<float> getShadowCascadeLevels(Camera camera);
    };
}

#endif //DREAM_OPENGLRENDERER_H
