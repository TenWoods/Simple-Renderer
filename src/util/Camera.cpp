#include "Camera.h"
#include <iostream>

namespace SRenderer
{
    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;
    const float SPEED       =  20.0f;
    const float SENSITIVITY =  0.1f;
    const float ZOOM        =  45.0f;
    const float NEAR        =  0.1f;
    const float FAR         =  1000.0f;

    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), near(NEAR), far(FAR)
    {
        this->position = position;
        worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateVector();
    }

    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), near(NEAR), far(FAR)
    {
        this->position = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateVector();
    }

    void Camera::set_far(float value)
    {
        far = value;
    }

    float Camera::get_far() const
    {
        return far;
    }

    void Camera::set_near(float value)
    {
        near = value;
    }

    float Camera::get_near() const
    {
        return near;
    }

    glm::mat4 Camera::get_ViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 Camera::get_Projection(int width, int height)
    {
        return glm::perspective(glm::radians(zoom), (float)width / (float)height, near, far);
    }

    void Camera::move(SRenderer::Direction dir, float deltaTime)
    {
        float velocity = moveSpeed * deltaTime;
        switch (dir)
        {
            case Direction::Forward:
                position += front * velocity;
                break;
            case Direction::Backward:
                position -= front * velocity;
                break;
            case Direction::Right:
                position += right * velocity;
                break;
            case Direction::Left:
                position -= right * velocity;
                break;
            case Direction::Up:
                position += up * velocity;
                break;
            case Direction::Down:
                position -= up * velocity;
                break;
            default:
                break;
        }
//        std::cout << deltaTime << std::endl;
//        std::cout << position.x << ',' << position.y << ',' << position.z << std::endl;
    }

    void Camera::rotate(float xoffset, float yoffset, GLboolean constrainPitch)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        yaw += xoffset;
        pitch += yoffset;
        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
        updateVector();
    }

    void Camera::set_Zoom(float value)
    {
        zoom = value;
    }

    float Camera::get_Zoom() const
    {
        return zoom;
    }

    void Camera::set_MouseSpeed(float value)
    {
        moveSpeed = value;
    }

    float Camera::get_MouseSpeed() const
    {
        return moveSpeed;
    }

    void Camera::set_MouseSensitivity(float value)
    {
        mouseSensitivity = value;
    }

    float Camera::get_MouseSensitivity() const
    {
        return mouseSensitivity;
    }

    glm::vec3 Camera::get_Position() const
    {
        return position;
    }

    void Camera::updateVector()
    {
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
}