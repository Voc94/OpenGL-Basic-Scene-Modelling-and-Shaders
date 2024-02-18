#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
namespace gps {

    // Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraOrientation, glm::vec3 cameraUpDirection, float speed)
        : cameraPosition(cameraPosition), cameraOrientation(cameraOrientation), cameraUpDirection(cameraUpDirection), cameraSpeed(speed) {
    }

    // Return the view matrix using the glm::lookAt function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraOrientation, cameraUpDirection);
    }

    // Update camera position based on the camera's orientation
    void Camera::move(MOVE_DIRECTION direction) {
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraSpeed * cameraOrientation;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraSpeed * cameraOrientation;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraSpeed * glm::normalize(glm::cross(cameraOrientation, cameraUpDirection));
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraSpeed * glm::normalize(glm::cross(cameraOrientation, cameraUpDirection));
            break;
        }
    }

    // Update camera orientation based on mouse movement
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraOrientation = glm::normalize(front);
    }

}

