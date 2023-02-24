//
// Created by Deepak Ramalingam on 12/31/22.
//

#include "dream/renderer/OpenGLPhysicsDebugDrawer.h"
#include <iostream>

namespace Dream {
    void OpenGLPhysicsDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
        // TODO: also pass along color to shader

        // draws a simple line of pixels between points but stores them for later draw
        Line line;
        line.p1 = { (float) from.getX(), (float) from.getY(), (float) from.getZ() };
        line.p2 = { (float) to.getX(), (float) to.getY(), (float) to.getZ() };
        lines.push_back(line);
    }

    void OpenGLPhysicsDebugDrawer::setDebugMode(int p) {
        m = p;
    }

    int OpenGLPhysicsDebugDrawer::getDebugMode(void) const {
        return m;
    }

    void OpenGLPhysicsDebugDrawer::drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {

    }

    void OpenGLPhysicsDebugDrawer::reportErrorWarning(const char *) {

    }

    void OpenGLPhysicsDebugDrawer::draw3dText(const btVector3 &, const char *) {

    }

    #ifndef EMSCRIPTEN
    btIDebugDraw::DefaultColors OpenGLPhysicsDebugDrawer::getDefaultColors() const {
        btIDebugDraw::DefaultColors defaultColors;
        defaultColors.m_wantsDeactivationObject = btVector3(0.0, 1.0, 0.0);
        defaultColors.m_disabledSimulationObject = btVector3(0.0, 1.0, 0.0);
        defaultColors.m_disabledDeactivationObject = btVector3(0.0, 1.0, 0.0);
        defaultColors.m_deactivatedObject = btVector3(1.0, 0.0, 0.0);
        defaultColors.m_contactPoint = btVector3(1.0, 1.0, 0.0);
        defaultColors.m_activeObject = btVector3(1.0, 0.0, 0.0);
        defaultColors.m_aabb = btVector3(0.0, 1.0, 0.0);
        return defaultColors;
    }
    #endif

    void OpenGLPhysicsDebugDrawer::doDrawing() {
        unsigned int vao = 0;
        unsigned int vbo = 0;

        // initialize object IDs if not configured before
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        // configure vertex attributes (only on vertex data size() > 0)
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(lines) * 2, &lines[0], GL_STATIC_DRAW);

        // calculate stride from number of non-empty vertex attribute arrays
        size_t stride = 3 * sizeof(float);  // positions

        // positions
        size_t offset = 0;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *) offset);
        offset += 3 * sizeof(float);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, (int) lines.size() * 2);
        glBindVertexArray(0);

        glDeleteBuffers(0, &vbo);
        glDeleteBuffers(0, &vao);

        lines.clear();
    }
}
