#ifndef SIMPLE_RENDERER_CAMERA_H
#define SIMPLE_RENDERER_CAMERA_H
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        float near;
        float far;
    public:
        Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
        glm::mat4 get_ViewMatrix();
        glm::mat4 get_Projection(int width, int height, bool isOrtho);

        glm::mat4 get_invProjection(int width, int height);
        glm::mat4 get_invView();

        void move(Direction dir, float deltaTime);
        void rotate(float xoffset, float yoffset, GLboolean constrainPitch = true);

        void set_near(float value);
        float get_near() const;

        void set_far(float value);
        float get_far()const;

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
