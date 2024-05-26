#include <camera.h>
#include <utils.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), keySensitivity(KEYSENSITIVITY)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction)
{
    if (direction == FORWARD)
        Position += Front * KEYSENSITIVITY;
    if (direction == BACKWARD)
        Position -= Front * KEYSENSITIVITY;
    if (direction == LEFT)
        Position -= Right * KEYSENSITIVITY;
    if (direction == RIGHT)
        Position += Right * KEYSENSITIVITY;
    if (direction == UP)
        Position += Up * KEYSENSITIVITY;
    if (direction == DOWN)
        Position -= Up * KEYSENSITIVITY;

    // for rotation, change up
    if (direction == CLOCKWISE) {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(10 * KEYSENSITIVITY), Front);
        Up = glm::vec3(rotationMatrix * glm::vec4(Up, 1.0f));
    }
    if (direction == ANTICLOCKWISE) {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-10 * KEYSENSITIVITY), Front);
        Up = glm::vec3(rotationMatrix * glm::vec4(Up, 1.0f));
    }
    
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch, GLboolean constrainYaw)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset * 0.1f;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f; 
}

void Camera::updateCameraVectors()
{
    // calculate the Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // calculate the Right vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    // calculate the Up vector
    Up = glm::normalize(glm::cross(Right, Front));
}

