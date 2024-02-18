#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {
    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_LEFT, MOVE_RIGHT };

    class Camera {
    public:
        glm::vec3 cameraPosition;
        glm::vec3 cameraOrientation;
        glm::vec3 cameraUpDirection;
        float cameraSpeed;

        Camera(glm::vec3 cameraPosition, glm::vec3 cameraOrientation, glm::vec3 cameraUpDirection, float speed);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction);
        void rotate(float pitch, float yaw);
        // Retrieve current camera position
        glm::vec3 getPosition() const {
            return cameraPosition;
        }

        // Set camera position
        void setPosition(const glm::vec3& newPosition) {
            cameraPosition = newPosition;
        }

        // Retrieve current camera orientation
        glm::vec3 getOrientation() const {
            return cameraOrientation;
        }

        // Set camera orientation
        void setOrientation(const glm::vec3& newOrientation) {
            cameraOrientation = newOrientation;
        }
    };
}


#endif /* Camera_hpp */
