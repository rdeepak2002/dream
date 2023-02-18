//
// Created by Deepak Ramalingam on 2/17/23.
//

#include "dream/renderer/OpenGLBaseTerrain.h"
#include "ogldev/ogldev_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <string.h>

namespace Dream {

    OpenGLBaseTerrain::OpenGLBaseTerrain(float worldScale) {
//        if (!m_terrainTech.init()) {
//            printf("Error initializing tech\n");
//            exit(0);
//        }

        m_worldScale = worldScale;
    }

    void OpenGLBaseTerrain::render() {
//        auto projection = camera.getProjectionMatrix();
//        auto view = camera.getViewMatrix();
//        Matrix4f VP = Camera.GetViewProjMatrix();

//        m_terrainTech.enable();
//        m_terrainTech.SetVP(VP);
//        m_terrainTech.setProjection(projection);
//        m_terrainTech.setView(view);

        // TODO: pass in projection and view to shader


        m_triangleList.render();
    }

    void OpenGLBaseTerrain::loadFromFile(const char *pFilename) {
        loadHeightMapFile(pFilename);

        m_triangleList.createTriangleList(m_terrainSize, m_terrainSize, this);
    }

    float OpenGLBaseTerrain::getHeight(int x, int z) const {
        return m_heightMap.Get(x, z);
    }

    float OpenGLBaseTerrain::getWorldScale() const {
        return m_worldScale;
    }

    void OpenGLBaseTerrain::loadHeightMapFile(const char *pFilename) {
        int FileSize = 0;
        unsigned char* p = (unsigned char*)ReadBinaryFile(pFilename, FileSize);

        if (FileSize % sizeof(float) != 0) {
            printf("%s:%d - '%s' does not contain an whole number of floats (size %d)\n", __FILE__, __LINE__, pFilename, FileSize);
            exit(0);
        }

        m_terrainSize = (int)sqrtf((float)FileSize / (float)sizeof(float));

        printf("Terrain size %d\n", m_terrainSize);

        if ((m_terrainSize * m_terrainSize) != (FileSize / sizeof(float))) {
            printf("%s:%d - '%s' does not contain a square height map - size %d\n", __FILE__, __LINE__, pFilename, FileSize);
            exit(0);
        }

        m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, (float*)p);
    }

}