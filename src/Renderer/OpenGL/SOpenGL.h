#ifndef SIMPLE_RENDERER_SOPENGL_H
#define SIMPLE_RENDERER_SOPENGL_H
#include "../../util/Shader.h"
#include <GLFW/glfw3.h>
#include <memory>
#include "../../objects/Model.hpp"
#include "../../util/Camera.h"

namespace SRenderer
{
    class SOpenGL
    {
    private:
        GLFWwindow* window;
        const int WIDTH = 800;
        const int HEIGHT = 600;
        std::vector<std::shared_ptr<SObject>> scene_root;
        Shader m_shader;
        Camera mainCamera;
        float lastFrame;
        float deltaTime;
    private:
        void initWindow();
        void renderLoop();
        void release();
        void processInput(GLFWwindow* window);
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    public:
        SOpenGL();
        void run();
        void addObject(std::string path);
    };
}



#endif //SIMPLE_RENDERER_SOPENGL_H
