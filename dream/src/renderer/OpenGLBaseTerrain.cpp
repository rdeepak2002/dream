//
// Created by Deepak Ramalingam on 2/17/23.
//

#include "dream/renderer/OpenGLBaseTerrain.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"
#include <ogldev/ogldev_util.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <string.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image_write.h>

namespace Dream {

    OpenGLBaseTerrain::OpenGLBaseTerrain(float worldScale, float textureScale) {
//        if (!m_terrainTech.init()) {
//            printf("Error initializing tech\n");
//            exit(0);
//        }

        m_worldScale = worldScale;
        m_textureScale = textureScale;

        textureDiffuse0 = new OpenGLTexture(Project::getPath().append("assets").append("botw-ground-textures").append("MaterialAlb_Slice_0_.png"));
        textureNormal0 = new OpenGLTexture(Project::getPath().append("assets").append("botw-ground-textures").append("MaterialCmb_Slice_0_.png"));

        textureDiffuse1 = new OpenGLTexture(Project::getPath().append("assets").append("botw-ground-textures").append("MaterialAlb_Slice_73_.png"));
        textureNormal1 = new OpenGLTexture(Project::getPath().append("assets").append("botw-ground-textures").append("MaterialCmb_Slice_73_.png"));

        textureDiffuse2 = new OpenGLTexture(Project::getPath().append("assets").append("botw-ground-textures").append("MaterialAlb_Slice_0_.png"));
        textureNormal2 = new OpenGLTexture(Project::getPath().append("assets").append("botw-ground-textures").append("MaterialCmb_Slice_0_.png"));
    }

    OpenGLBaseTerrain::~OpenGLBaseTerrain() {
        delete textureDiffuse0;
        delete textureNormal0;

        delete textureDiffuse1;
        delete textureNormal1;

        delete textureDiffuse2;
        delete textureNormal2;
    }

    void OpenGLBaseTerrain::render(OpenGLShader* shader) {
//        auto projection = camera.getProjectionMatrix();
//        auto view = camera.getViewMatrix();
//        Matrix4f VP = Camera.GetViewProjMatrix();

//        m_terrainTech.enable();
//        m_terrainTech.SetVP(VP);
//        m_terrainTech.setProjection(projection);
//        m_terrainTech.setView(view);

        // TODO: pass in projection and view to shader

        shader->setFloat("gMinHeight", m_minHeight);
        shader->setFloat("gMaxHeight", m_maxHeight);
        shader->setFloat("gHeight0", 80.0f);
        shader->setFloat("gHeight1", 110.0f);

        shader->setInt("textureDiffuse0", 0);
        textureDiffuse0->bind(0);
        shader->setInt("textureNormal0", 1);
        textureNormal0->bind(1);

        shader->setInt("textureDiffuse1", 2);
        textureDiffuse1->bind(2);
        shader->setInt("textureNormal1", 3);
        textureNormal1->bind(3);

        shader->setInt("textureDiffuse2", 4);
        textureDiffuse2->bind(4);
        shader->setInt("textureNormal2", 5);
        textureNormal2->bind(5);

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

        for (int x = 0; x < m_terrainSize; x++) {
            for (int y = 0; y < m_terrainSize; y++) {
//                int cx = m_terrainSize / 2;
//                int cy = m_terrainSize / 2;
//                int dx = x - cx;
//                int dy = y - cy;
//                float dist = sqrt(dx * dx + dy * dy);
//                m_heightMap.Set(x, y, 100.0f * sin(0.01 * dist) + 50.0f);
//                m_heightMap.Set(x, y, 0.0f);
            }
        }

        m_minHeight = 0;
        for (int x = 0; x < m_terrainSize; x++) {
            for (int y = 0; y < m_terrainSize; y++) {
                m_maxHeight = max(m_maxHeight, m_heightMap.Get(x, y));
            }
        }
    }

    float OpenGLBaseTerrain::getSize() const {
        return m_terrainSize;
    }

    float OpenGLBaseTerrain::getTextureScale() const {
        return m_textureScale;
    }

    float OpenGLBaseTerrain::getHeight(glm::vec2 vec) const {
        float x = vec.x;
        float y = vec.y;
        if ((int) x < 0) {
            x = 0;
        }
        if ((int) y < 0) {
            y = 0;
        }
        if ((int) x >= m_terrainSize) {
            x = m_terrainSize - 1;
        }
        if ((int) y >= m_terrainSize) {
            y = m_terrainSize - 1;
        }
        return m_heightMap.Get((int) x, (int) y);
    }

    void OpenGLBaseTerrain::saveToFile(const char *pFilename) {
//        void WriteBinaryFile(const char* pFilename, const void* pData, int size);
//        WriteBinaryFile(pFilename, m_heightMap.GetBaseAddr(), m_heightMap.GetSize() * sizeof(float));

//        unsigned char* p = (unsigned char*)malloc(m_terrainSize * m_terrainSize);
//        float* src = m_heightMap.GetBaseAddr();
//        for (int i = 0; i < m_terrainSize * m_terrainSize; i++) {
//            float f = src[i];
//            p[i] = (unsigned char)(f * 255.0f);
//        }
//
//        WriteBinaryFile(pFilename, p, m_terrainSize * m_terrainSize);

//        unsigned char* p = (unsigned char*)malloc(m_terrainSize * m_terrainSize);
//
//        float* src = m_heightMap.GetBaseAddr();
//
//        float Delta = m_maxHeight - m_minHeight;
//
//        for (int i = 0; i < m_terrainSize * m_terrainSize; i++) {
//            float f = (src[i] - m_minHeight) / Delta;
//            p[i] = (unsigned char)(f * 255.0f);
//        }
//
//        stbi_write_png("heightmap.png", m_terrainSize, m_terrainSize, 1, p, m_terrainSize);
//        free(p);
    }
}