//
// Created by Deepak Ramalingam on 2/17/23.
//

#ifndef DREAM_OPENGLBASETERRAIN_H
#define DREAM_OPENGLBASETERRAIN_H

#include "dream/renderer/Camera.h"
#include "dream/renderer/OpenGLTriangleList.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/renderer/OpenGLShader.h"
#include <ogldev/ogldev_array_2d.h>

namespace Dream {
    class OpenGLBaseTerrain {
    public:
        OpenGLBaseTerrain(float worldScale, float textureScale);

        ~OpenGLBaseTerrain();

        void render(OpenGLShader* shader);

        void loadFromFile(const char* pFilename);

        void refreshTerrainTriangleList();

        void saveToFile(const char* pFilename);

        float getHeight(glm::vec2 vec) const;

        float getHeight(int x, int z) const;

        void setHeight(int x, int z, float y);

        float getWorldScale() const;

        float getSize() const;

        float getTextureScale() const;
    protected:
        void loadHeightMapFile(const char* pFilename);
        int m_terrainSize = 0;
        float m_textureScale = 1.0f;
        float m_worldScale = 1.0f;
        float m_minHeight = 0.0f;
        float m_maxHeight = 0.0f;
        Array2D<float> m_heightMap;
        OpenGLTriangleList m_triangleList;

        OpenGLTexture *textureDiffuse0;
        OpenGLTexture *textureNormal0;

        OpenGLTexture *textureDiffuse1;
        OpenGLTexture *textureNormal1;

        OpenGLTexture *textureDiffuse2;
        OpenGLTexture *textureNormal2;
    };
}

#endif //DREAM_OPENGLBASETERRAIN_H
