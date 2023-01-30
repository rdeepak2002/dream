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
#include "dream/renderer/OpenGLTexture.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLSphereMesh.h"
#include "dream/renderer/Mesh.h"
#include "dream/scene/Entity.h"
#include "dream/renderer/OpenGLSkybox.h"
#include "dream/renderer/OpenGLShadowMapFBO.h"
#include "dream/renderer/Camera.h"

namespace Dream {
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();

        ~OpenGLRenderer();

        void render(int viewportWidth, int viewportHeight, bool fullscreen) override;

        unsigned int getOutputRenderTexture() override;
    private:
        const int RENDER_FLAG_SHADOW = 1;
        const int RENDER_FLAG_FINAL = 2;
        int SHADOW_WIDTH = 1024 * 8;
        int SHADOW_HEIGHT = 1024 * 8;
        void drawScene(std::optional<Camera> maybeCamera, OpenGLShader* shader, int flags);
        void drawMesh(OpenGLMesh* openGLMesh);

        OpenGLFrameBuffer *outputRenderTextureFbo;
        OpenGLShadowMapFBO* shadowMapFbo;
        OpenGLShader* simpleLightingShader;
        OpenGLShader* simpleDepthShader;
        OpenGLMesh* cubeMesh;
    };
}

#endif //DREAM_OPENGLRENDERER_H
