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
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
    }

    void SOpenGL::renderLoop()
    {
#ifdef __linux__
        m_shader = Shader("../resource/shaders/svertex.vert", "../resource/shaders/gbuffer.frag");
        quad_shader = Shader("../resource/shaders/quad.vert", "../resource/shaders/ssr.frag");
        addModel("../resource/model/Sponza/glTF/Sponza.gltf");
        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
#elif _WIN64
        m_shader = Shader("../../resource/shaders/svertex.vert", "../../resource/shaders/gbuffer.frag");
        quad_shader = Shader("../../resource/shaders/deferred.vert", "../../resource/shaders/deferred.frag");
        addModel("../../resource/model/sponza/Sponza.gltf");
        addModel("../../resource/model/bottle/WaterBottle.gltf");
        addModel("../../resource/model/bottle/WaterBottle.gltf");
        addModel("../../resource/model/bottle/WaterBottle.gltf");
        addModel("../../resource/model/bottle/WaterBottle.gltf");
        addModel("../../resource/model/bottle/WaterBottle.gltf");

#endif
        quad_shader.use();
        quad_shader.setInt("ColorBuffer", 0);
        quad_shader.setInt("NormalBuffer", 1);
        quad_shader.setInt("DepthBuffer", 2);
        scene_root[0]->set_scale(glm::vec3(0.1f, 0.1f, 0.1f));
        scene_root[0]->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
        scene_root[1]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
        scene_root[1]->set_position(glm::vec3(0.0f, 6.0f, 0.0f));
        scene_root[2]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
        scene_root[2]->set_position(glm::vec3(15.0f, 6.0f, 0.0f));
        scene_root[3]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
        scene_root[3]->set_position(glm::vec3(-15.0f, 6.0f, 0.0f));
        scene_root[4]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
        scene_root[4]->set_position(glm::vec3(0.0f, 6.0f, 10.0f));
        scene_root[5]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
        scene_root[5]->set_position(glm::vec3(15.0f, 6.0f, -10.0f));
        addLight(SLight(glm::vec3(1.0, 20.0, 0.0), glm::vec3(255.0, 255.0, 255.0)));
        deferredRendering();
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
            mainCamera.rotate(-4.0, 0.0);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            mainCamera.rotate(4.0, 0.0);
    }

    void SOpenGL::set_light()
    {
        m_shader.use();
        //TODO: support multi-light
        m_shader.setVec3("lightPos", lights[0].get_position());
        m_shader.setVec3("lightColor", lights[0].get_color());
//        std::cout << lights[0].get_position().x << ',' << lights[0].get_position().y << ','  << lights[0].get_position().z;
//        std::cout << lights[0].get_color().x << ',' << lights[0].get_color().y << ',' << lights[0].get_color().z << std::endl;
    }

    void SOpenGL::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    SOpenGL::SOpenGL() : mainCamera(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 0.0f, 0.0f),
                         lastFrame(0.0), deltaTime(0.0)

    {

    }

    void SOpenGL::run()
    {
        initWindow();
        renderLoop();
        release();
    }

    void SOpenGL::deferredRendering()
    {
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
        };
        unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        unsigned int deferredFrameBuffer;
        glGenFramebuffers(1, &deferredFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBuffer);
        unsigned int GBuffer[3];
        glGenTextures(3, GBuffer);
        for (int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, GBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, GBuffer[i], 0);
        }
        glBindTexture(GL_TEXTURE_2D, GBuffer[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GBuffer[2], 0);
        unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);

        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            glfwPollEvents();
            processInput(window);
            glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBuffer);
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_shader.use();
            glm::mat4 projection = mainCamera.get_Projection(WIDTH, HEIGHT);
            glm::mat4 view = mainCamera.get_ViewMatrix();
            m_shader.setMat4("projection", projection);
            m_shader.setMat4("view", view);
            m_shader.setVec3("cameraPos", mainCamera.get_Position());
            quad_shader.use();
            quad_shader.setMat4("inverseProj", mainCamera.get_invProjection(WIDTH, HEIGHT));
            quad_shader.setMat4("inverseView", mainCamera.get_invView());
            quad_shader.setMat4("projection", projection);
            quad_shader.setMat4("view", view);
            quad_shader.setFloat("time", glfwGetTime());
            set_light();
            //m_shader.setMat4("model", glm::mat4(1.0f));
            //scene_root[1]->draw(m_shader);
            for (auto& object : scene_root)
            {
                //std::cout << "Draw" << std::endl;
                object->draw(m_shader);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            quad_shader.use();
            for (int i = 0; i < 3; i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, GBuffer[i]);
            }
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            glfwSwapBuffers(window);
            glfwPollEvents();
            std::cout << "FPS: " << 1.0 / deltaTime << std::endl;
        }
    }

    void SOpenGL::forwardRendering()
    {
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
            m_shader.setVec3("cameraPos", mainCamera.get_Position());
            set_light();
            //m_shader.setMat4("model", glm::mat4(1.0f));
            //scene_root[1]->draw(m_shader);
            for (auto& object : scene_root)
            {
                //std::cout << "Draw" << std::endl;
                object->draw(m_shader);
            }
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void SOpenGL::addModel(std::string path)
    {
        scene_root.emplace_back(std::make_shared<Model>(path));
    }

    void SOpenGL::addLight(SRenderer::SLight light)
    {
        lights.emplace_back(light);
    }
}
