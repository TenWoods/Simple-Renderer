#ifndef SIMPLE_RENDERER_CAMERA_H
#define SIMPLE_RENDERER_CAMERA_H
#include <glad/glad.h>
#include <vec3.hpp>
#include <matrix.hpp>
#include <gtc/matrix_transform.hpp>

namespace SRenderer
{
    enum class Direction
    {
        Forward,
        Backward,
        Right,
        Left,
        Up,
        Down
    };
    class Camera
    {
    private :
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;
        float moveSpeed;
        float mouseSensitivity;
        float zoom;
        float pitch;
        float yaw;
    public:
        Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
        glm::mat4 get_ViewMatrix();
        void move(Direction dir, float deltaTime);
        void rotate(float xoffset, float yoffset, GLboolean constrainPitch = true);

        void set_Zoom(float value);
        [[nodiscard]] float get_Zoom() const;

        void set_MouseSpeed(float value);
        [[nodiscard]] float get_MouseSpeed() const;

        void set_MouseSensitivity(float value);
        [[nodiscard]] float get_MouseSensitivity() const;

        [[nodiscard]] glm::vec3 get_Position() const;
    private:
        void updateVector();
    };

} // SRenderer

#endif //SIMPLE_RENDERER_CAMERA_H
