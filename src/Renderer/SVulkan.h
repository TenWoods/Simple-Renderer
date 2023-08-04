//
// Created by jukis on 2023/08/04.
//

#ifndef SIMPLE_RENDERER_SVULKAN_H
#define SIMPLE_RENDERER_SVULKAN_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace SVulkan
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    class SVulkan
    {
    private:
        GLFWwindow* window;
    private:
        void initWindow();
        void initVulkan();
        void renderLoop();
        void release();
    public:
        void run();
    };

} // SVulkan

#endif //SIMPLE_RENDERER_SVULKAN_H
