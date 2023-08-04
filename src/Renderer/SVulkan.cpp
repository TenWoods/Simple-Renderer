//
// Created by jukis on 2023/08/04.
//

#include "SVulkan.h"

namespace SVulkan
{
    void SVulkan::run()
    {
        initWindow();
        initVulkan();
        renderLoop();
        release();
    }

    void SVulkan::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "SRenderer", nullptr, nullptr);
    }

    void SVulkan::initVulkan()
    {

    }
} // SVulkan