//
// Created by Deepak Ramalingam on 2/17/23.
//

#ifndef DREAM_OPENGLBASETERRAIN_H
#define DREAM_OPENGLBASETERRAIN_H

#include "Camera.h"
#include "OpenGLTriangleList.h"
#include <ogldev/ogldev_array_2d.h>

namespace Dream {
    class OpenGLBaseTerrain {
    public:
        OpenGLBaseTerrain(float worldScale);

        void render(Camera& camera);

        void loadFromFile(const char* pFilename);

        float getHeight(int x, int z) const;

        float getWorldScale() const;
    protected:
        void loadHeightMapFile(const char* pFilename);
        int m_terrainSize = 0;
        float m_worldScale = 1.0f;
        Array2D<float> m_heightMap;
        OpenGLTriangleList m_triangleList;
//        OpenGLTerrainTechnique m_terrainTech;
    };
}

#endif //DREAM_OPENGLBASETERRAIN_H
