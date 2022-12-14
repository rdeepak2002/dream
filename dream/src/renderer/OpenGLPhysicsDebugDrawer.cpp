//
// Created by Deepak Ramalingam on 12/31/22.
//

#include "dream/renderer/OpenGLPhysicsDebugDrawer.h"
#include <iostream>

namespace Dream {
    void OpenGLPhysicsDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
        // Vertex data
        GLfloat points[12];

        points[0] = -1 * from.x();
        points[1] = -1 * from.y();
        points[2] = from.z();
        points[3] = color.x();
        points[4] = color.y();
        points[5] = color.z();

        points[6] = -1 * to.x();
        points[7] = -1 * to.y();
        points[8] = to.z();
        points[9] = color.x();
        points[10] = color.y();
        points[11] = color.z();

        glLineWidth(1.0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
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
}
