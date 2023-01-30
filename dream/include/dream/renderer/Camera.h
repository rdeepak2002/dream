//
// Created by Deepak Ramalingam on 1/29/23.
//

#ifndef DREAM_CAMERA_H
#define DREAM_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Dream {
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;
    const float SPEED       =  2.5f;
    const float SENSITIVITY =  0.1f;
    const float ZOOM        =  45.0f;

    class Camera {
        public:
            // camera attributes
            glm::vec3 Position;
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;
            glm::vec3 WorldUp;

            // euler Angles
            float Yaw;
            float Pitch;

            // camera options
            float MovementSpeed;
            float MouseSensitivity;
            float Zoom;

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
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

            // returns the view matrix calculated using Euler Angles and the LookAt Matrix
            glm::mat4 getViewMatrix();

            // returns the projection matrix
            glm::mat4 getProjectionMatrix();

            // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
            void processKeyboard(Camera_Movement direction, float deltaTime);

            // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
            void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

            // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
            void processMouseScroll(float yoffset);

            // update camera vectors
            void updateCameraVectors();
    };
}

#endif //DREAM_CAMERA_H
