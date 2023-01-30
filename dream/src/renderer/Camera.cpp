//
// Created by Deepak Ramalingam on 1/29/23.
//

#include "dream/renderer/Camera.h"

namespace Dream {
    Camera::Camera(float viewportWidth, float viewportHeight, float zNear, float zFar, float fov, glm::vec3 position,
                   glm::vec3 up, float yaw, float pitch)
                   : zNear(zNear), zFar(zFar), fov(fov), front(glm::vec3(0.0f, 0.0f, -1.0f)) {
        this->viewportWidth = viewportWidth;
        this->viewportHeight = viewportHeight;
        this->fov = fov;
        position = position;
        worldUp = up;
        yaw = yaw;
        pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void Camera::updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(yaw) * cos(pitch);
        front.y = sin(pitch);
        front.z = sin(yaw) * cos(pitch);
        front = glm::normalize(front);

        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up    = glm::normalize(glm::cross(right, front));
    }

    glm::mat4 Camera::getProjectionMatrix() {
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) viewportWidth / (float) viewportHeight, zNear, zFar);
        return projection;
    }
}