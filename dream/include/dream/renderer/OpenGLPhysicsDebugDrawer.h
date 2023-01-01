//
// Created by Deepak Ramalingam on 12/31/22.
//

#ifndef DREAM_OPENGLPHYSICSDEBUGDRAWER_H
#define DREAM_OPENGLPHYSICSDEBUGDRAWER_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <btBulletDynamicsCommon.h>
#include <glad/glad.h>

namespace Dream {
    class OpenGLPhysicsDebugDrawer : public btIDebugDraw {
    private:
        GLuint VBO, VAO;
    public:
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
        virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &);
        virtual void reportErrorWarning(const char *);
        virtual void draw3dText(const btVector3 &, const char *);
        virtual void setDebugMode(int p);
        int getDebugMode(void) const;
        [[nodiscard]] btIDebugDraw::DefaultColors getDefaultColors() const;
        int m;
    };
}

#endif //DREAM_OPENGLPHYSICSDEBUGDRAWER_H
