//
// Created by Deepak Ramalingam on 10/24/22.
//

#include "dream/renderer/OpenGLRenderer.h"
#include <iostream>
#include "dream/Project.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <glad/glad.h>
#else
#include <glad/glad.h>
#endif

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader program
        shader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                  Project::getPath().append("assets").append("shaders").append("shader.frag").c_str(),
                                  nullptr);

        screenShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("screen_shader.vert").c_str(),
                                  Project::getPath().append("assets").append("shaders").append("screen_shader.frag").c_str(),
                                  nullptr);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
                -0.5f, -0.5f, 0.0f, // left
                0.5f, -0.5f, 0.0f, // right
                0.0f,  0.5f, 0.0f  // top
        };

        //    unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        // screen quad VAO
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
        };
        unsigned int screenQuadVBO;
        glGenVertexArrays(1, &screenQuadVAO);
        glGenBuffers(1, &screenQuadVBO);
        glBindVertexArray(screenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        frameBuffer = new OpenGLFrameBuffer();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete this->shader;
        delete this->screenShader;
        delete this->frameBuffer;
    }

    unsigned int OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        frameBuffer->bind();

        this->resizeFrameBuffer();

//        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        #ifdef EMSCRIPTEN
        if (fullscreen) {
            auto dpiScale = 2;
            glViewport(0, 0, viewportWidth * dpiScale, viewportHeight * dpiScale);
        }
        #endif

        // clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        this->shader->use();
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

        // clear all relevant buffers
        glClearColor(0.1f, 0.105f, 0.11f, 1.0f); // set clear color to editor background
        glClear(GL_COLOR_BUFFER_BIT);

        if (fullscreen) {
            screenShader->use();
            glBindVertexArray(screenQuadVAO);
            frameBuffer->bindTexture();
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        return frameBuffer->getTexture();
    }

    void OpenGLRenderer::resizeFrameBuffer() {
        // resize framebuffer whenever there is a new viewport size
        GLint viewportWidth = this->getViewportDimensions().first;
        GLint viewportHeight = this->getViewportDimensions().second;
        if (frameBuffer->getWidth() != viewportWidth || frameBuffer->getHeight() != viewportHeight) {
            frameBuffer->resize(viewportWidth, viewportHeight);
        }
    }

    void OpenGLRenderer::printGLVersion() {
        printf("GL Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
        printf("GL Version:  %s\n", glGetString(GL_VERSION));
    }

    std::pair<int, int> OpenGLRenderer::getViewportDimensions() {
        GLint dims[4] = {0};
        glGetIntegerv(GL_VIEWPORT, dims);
        return std::make_pair(dims[2], dims[3]);
    }
}
