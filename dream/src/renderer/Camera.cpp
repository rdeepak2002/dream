//
// Created by Deepak Ramalingam on 1/29/23.
//

#include "dream/renderer/Camera.h"
#include <iostream>

namespace Dream {
    Camera::Camera(float viewportWidth, float viewportHeight, float zNear, float zFar, float fov, glm::vec3 position,
                   glm::vec3 up, float yaw, float pitch)
            : zNear(zNear), zFar(zFar), fov(fov), front(glm::vec3(0.0f, 0.0f, -1.0f)) {
        this->viewportWidth = viewportWidth;
        this->viewportHeight = viewportHeight;
        this->fov = fov;
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void Camera::updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(yaw) * cos(pitch);
        newFront.y = sin(pitch);
        newFront.z = sin(yaw) * cos(pitch);
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }

    glm::mat4 Camera::getProjectionMatrix() {
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) viewportWidth / (float) viewportHeight, zNear, zFar);
        return projection;
    }

    float Camera::getAspect() {
        return (float) viewportWidth / (float) viewportHeight;
    }
}