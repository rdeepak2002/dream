//
// Created by Deepak Ramalingam on 1/29/23.
//

#ifndef DREAM_CAMERA_H
#define DREAM_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Dream {
    class Camera {
        public:
            // camera attributes
            glm::vec3 position;
            glm::vec3 front;
            glm::vec3 up;
            glm::vec3 right;
            glm::vec3 worldUp;

            // euler Angles
            float yaw;
            float pitch;

            // viewport dimensions
            float viewportWidth;
            float viewportHeight;

            // near and far planes
            float zNear = 0.1f;
            float zFar = 100.0f;

            // fov
            float fov = 45.0f;

            Camera (float viewportWidth, float viewportHeight, float zNear = 0.1f, float zFar = 100.0f, float fov = 45.0f,
                    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -M_PI_2, float pitch = 0);

            // returns the view matrix calculated using Euler Angles and the LookAt Matrix
            glm::mat4 getViewMatrix();

            // returns the projection matrix
            glm::mat4 getProjectionMatrix();

            // update camera vectors
            void updateCameraVectors();
    };
}

#endif //DREAM_CAMERA_H
