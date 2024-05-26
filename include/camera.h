#ifndef CAMERA_H
#define CAMERA_H

#include "defines.h"
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    CLOCKWISE,
    ANTICLOCKWISE
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float KEYSENSITIVITY = 0.005f;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float keySensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch = PITCH);
    
    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch); 

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(Camera_Movement direction);

    void ProcessMouseMovement(float xoffset, float yoffset,
                              GLboolean constrainPitch = true,
                              GLboolean constrainYaw = true);

    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

#endif