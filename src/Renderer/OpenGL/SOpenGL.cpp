#include "SOpenGL.h"
#include <iostream>
#include <functional>

namespace SRenderer
{
    void SOpenGL::initWindow()
    {
        if (!glfwInit())
        {
            std::cout << "GLFW init error" << std::endl;
            exit(EXIT_FAILURE);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Simple Renderer", nullptr, nullptr);
        if (window == nullptr)
        {
            std::cout << "GLFW window error" << std::endl;
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        std::function<void(GLFWwindow*,double,double)>callback = [this](GLFWwindow* w, double x, double y)
        {
            mouse_callback(w, x, y);
        };
        //glfwSetCursorPosCallback(window, callback);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
    }

    void SOpenGL::renderLoop()
    {
        m_shader = Shader("../../resource/shaders/svertex.vert", "../../resource/shaders/sfragment.frag");
        addModel("../../resource/model/sponza/Sponza.gltf");
        scene_root[0]->set_scale(glm::vec3(0.5f, 0.5f, 0.5f));
        scene_root[0]->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
        addLight(SLight(glm::vec3(0.0, 5.0, 0.0), glm::vec3(1.0, 1.0, 1.0)));
        glEnable(GL_DEPTH_TEST);
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            glfwPollEvents();
            processInput(window);
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_shader.use();
            glm::mat4 projection = mainCamera.get_Projection(WIDTH, HEIGHT);
            m_shader.setMat4("projection", projection);
            m_shader.setMat4("view", mainCamera.get_ViewMatrix());
            //TODO: support multi-light
            m_shader.setVec3("lightPos", lights[0].get_position());
            //m_shader.setMat4("model", glm::mat4(1.0f));
            for (auto& object : scene_root)
            {
                //std::cout << "Draw" << std::endl;
                object->draw(m_shader);
            }
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void SOpenGL::release()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void SOpenGL::processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mainCamera.move(Direction::Forward, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mainCamera.move(Direction::Backward, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mainCamera.move(Direction::Left, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mainCamera.move(Direction::Right, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            mainCamera.move(Direction::Up, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            mainCamera.move(Direction::Down, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            mainCamera.rotate(-2.0, 0.0);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            mainCamera.rotate(2.0, 0.0);
    }

    void SOpenGL::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        mainCamera.rotate(xoffset, yoffset);
    }

    void SOpenGL::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    SOpenGL::SOpenGL() : mainCamera(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), 0.0f, 0.0f),
                         lastFrame(0.0), deltaTime(0.0)

    {

    }

    void SOpenGL::run()
    {
        initWindow();
        renderLoop();
        release();
    }

    void SOpenGL::addModel(std::string path)
    {
        scene_root.emplace_back(std::shared_ptr<Model>(new Model(path)));
    }

    void SOpenGL::addLight(SRenderer::SLight light)
    {
        lights.emplace_back(light);
    }
}
