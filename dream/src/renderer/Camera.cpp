//
// Created by Deepak Ramalingam on 1/29/23.
//

#include "dream/renderer/Camera.h"

namespace Dream {
    Camera::Camera(float viewportWidth, float viewportHeight, float zNear, float zFar, float fov, glm::vec3 position,
                   glm::vec3 up, float yaw, float pitch)
                   : zNear(zNear), zFar(zFar), fov(fov), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
                   MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        this->viewportWidth = viewportWidth;
        this->viewportHeight = viewportHeight;
        this->fov = fov;
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void Camera::processMouseScroll(float yoffset) {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void Camera::updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }

    glm::mat4 Camera::getProjectionMatrix() {
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) viewportWidth / (float) viewportHeight, zNear, zFar);
        return projection;
    }
}