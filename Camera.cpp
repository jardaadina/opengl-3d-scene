#include "Camera.hpp"

namespace gps {

    // Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        // Calculate front and right directions of the camera
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    glm::mat4 Camera::getProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    // Returns the view matrix using glm::lookAt()
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    // Returns the current camera position
    glm::vec3 Camera::getCameraPosition() const {
        return cameraPosition;
    }

    // Updates camera parameters based on movement direction and speed
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        speed *= 3;
        if (direction == MOVE_FORWARD) {
            cameraPosition += cameraFrontDirection * speed;
        }
        if (direction == MOVE_BACKWARD) {
            cameraPosition -= cameraFrontDirection * speed;
        }
        if (direction == MOVE_RIGHT) {
            cameraPosition += cameraRightDirection * speed;
        }
        if (direction == MOVE_LEFT) {
            cameraPosition -= cameraRightDirection * speed;
        }
        if (direction == MOVE_UP) {
            cameraPosition += cameraUpDirection * speed;
        }
        if (direction == MOVE_DOWN) {
            cameraPosition -= cameraUpDirection * speed;
        }
    }

    // Updates camera parameters based on yaw and pitch rotation
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        // Normalize the front direction vector
        cameraFrontDirection = glm::normalize(front);

        // Update right and up directions based on the new front direction
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}