#include "SOpenGL.h"
#include <iostream>
#include <functional>

namespace SRenderer
{
    SOpenGL::SOpenGL() : mainCamera(glm::vec3(-70.0, 20.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 0.0f, 0.0f),
                         lightCamera(glm::vec3(0.0, 30.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 0.0, -50.0f),
                         lastFrame(0.0), deltaTime(0.0)
    {

    }
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
        gbuffer_shader = Shader("../resource/shaders/svertex.vert", "../resource/shaders/gbuffer.frag");
        direct_shader = Shader("../resource/shaders/quad.vert", "../resource/shaders/direct.frag");
        hiz_shader = Shader("../resource/shaders/quad.vert", "../resource/shaders/hizbuffer.frag");
        blur_shader = Shader("../resource/shaders/quad.vert", "../resource/shaders/blur.frag");
        ssr_shader = Shader("../resource/shaders/quad.vert", "../resource/shaders/hiztrace.frag");
        shadowMap_shader = Shader("../resource/shaders/lightDepth.vert", "../resource/shaders/lightDepth.frag");
        blur1_shader = Shader("../resource/shaders/SSABSS/blur1.vert", "../resource/shaders/SSABSS/blur1.frag");
        blur1_shader = Shader("../resource/shaders/SSABSS/blur2.vert", "../resource/shaders/SSABSS/blur2.frag");
        blur1_shader = Shader("../resource/shaders/SSABSS/blur3.vert", "../resource/shaders/SSABSS/blur3.frag");
        preCal_shader = Shader("../resource/shaders/SSABSS/PreCalculation.vert", "../resource/shaders/SSABSS/PreCalculation.frag");
        //shadow_shader = Shader("../resource/shaders/quad.vert", "../resource/shaders/PCSS.frag");
        addModel("../resource/model/Sponza/glTF/Sponza.gltf");
//        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
        addModel("../resource/model/AGAME/ABeautifulGame.gltf");
//        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
//        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
//        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
//        addModel("../resource/model/WaterBottle/glTF/WaterBottle.gltf");
#elif _WIN64
        gbuffer_shader = Shader("../../resource/shaders/svertex.vert", "../../resource/shaders/gbuffer.frag");
        direct_shader = Shader("../../resource/shaders/quad.vert", "../../resource/shaders/direct.frag");
        hiz_shader = Shader("../../resource/shaders/quad.vert", "../../resource/shaders/hizbuffer.frag");
        blur_shader = Shader("../../resource/shaders/quad.vert", "../../resource/shaders/blur.frag");
        ssr_shader = Shader("../../resource/shaders/quad.vert", "../../resource/shaders/hiztrace.frag");
        shadowMap_shader = Shader("../../resource/shaders/lightDepth.vert", "../../resource/shaders/lightDepth.frag");
        visibility_shader = Shader("../../resource/shaders/quad.vert", "../../resource/shaders/genVisibility.frag");
        addModel("../../resource/model/sponza/Sponza.gltf");
        addModel("../../resource/model/game/ABeautifulGame.gltf");
//        addModel("../../resource/model/bottle/WaterBottle.gltf");
//        addModel("../../resource/model/bottle/WaterBottle.gltf");
//        addModel("../../resource/model/bottle/WaterBottle.gltf");
//        addModel("../../resource/model/bottle/WaterBottle.gltf");

#endif
        preCal_shader.use();
        preCal_shader.setInt("m_SSABSSshadowMap", 0);
        blur1_shader.use();
        blur1_shader.setInt("m_SSABSSPreCal1", 0);
        blur1_shader.setInt("m_SSABSSPreCal2", 1);
        blur2_shader.use();
        blur2_shader.setInt("m_SSABSSBlur11", 0);
        blur2_shader.setInt("m_SSABSSBlur12", 1);
        blur2_shader.setInt("m_SSABSSPreCal2", 2);
        blur3_shader.use();
        blur3_shader.setInt("m_SSABSSBlur21", 0);
        blur3_shader.setInt("m_SSABSSBlur22", 1);
        blur3_shader.setInt("m_SSABSSPreCal2", 2);
        lightCamera.set_Zoom(90.0f);
        //ssr_shader.use();
        //hiz_shader.use();
        scene_root[0]->set_scale(glm::vec3(0.1f, 0.1f, 0.1f));
        scene_root[0]->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
        scene_root[1]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
        scene_root[1]->set_position(glm::vec3(0.0f, 6.0f, 0.0f));
//        scene_root[2]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
//        scene_root[2]->set_position(glm::vec3(15.0f, 6.0f, 0.0f));
//        scene_root[3]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
//        scene_root[3]->set_position(glm::vec3(-15.0f, 6.0f, 0.0f));
//        scene_root[4]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
//        scene_root[4]->set_position(glm::vec3(0.0f, 6.0f, 10.0f));
//        scene_root[5]->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
//        scene_root[5]->set_position(glm::vec3(15.0f, 6.0f, -10.0f));
        addLight(SLight(glm::vec3(0.0, 20.0, 0.0), glm::vec3(1000.0, 1000.0, 1000.0)));

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
        gbuffer_shader.use();
        //TODO: support multi-light
        gbuffer_shader.setVec3("lightPos", lights[0].get_position());
        gbuffer_shader.setVec3("lightColor",lights[0].get_color());
//        std::cout << lights[0].get_position().x << ',' << lights[0].get_position().y << ','  << lights[0].get_position().z;
//        std::cout << lights[0].get_color().x << ',' << lights[0].get_color().y << ',' << lights[0].get_color().z << std::endl;
    }

    void SOpenGL::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
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
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        //init G-buffer Pass
        glGenFramebuffers(1, &gbufferPass);
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferPass);

        glGenTextures(3, GBuffer);
        glGenTextures(1, &worldPosition);
        //Generate Color and Normal buffer
//        for (int i = 0; i < 2; i++)
//        {
//            glBindTexture(GL_TEXTURE_2D, GBuffer[i]);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, GBuffer[i], 0);
//        }
        //Color buffer
        glBindTexture(GL_TEXTURE_2D, GBuffer[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //Normal buffer
        glBindTexture(GL_TEXTURE_2D, GBuffer[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        //Generate Depth buffer
        levelsCount = 7;
        glBindTexture(GL_TEXTURE_2D, GBuffer[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelsCount - 1);
        glGenerateMipmap(GL_TEXTURE_2D);

        //View Position Buffer
        glBindTexture(GL_TEXTURE_2D, worldPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GBuffer[0], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GBuffer[1], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GBuffer[2], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, worldPosition, 0);

        unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(4, attachments);

        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//            std::cout << "Framebuffer not complete!" << std::endl;

        //init direct pass
        glGenFramebuffers(1, &directPass);
        glBindFramebuffer(GL_FRAMEBUFFER, directPass);
        glGenTextures(1, &directResult);
        glGenTextures(1, &viewPosition);
        glBindTexture(GL_TEXTURE_2D, directResult);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelsCount - 1);
        glGenerateMipmap(GL_TEXTURE_2D);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, directResult, 0);
        glBindTexture(GL_TEXTURE_2D, viewPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, viewPosition, 0);
        //GLuint direct_attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//            std::cout << "Framebuffer not complete!" << std::endl;

        //init shadow map pass
        glGenFramebuffers(1, &shadowMapPass);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapPass);
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        //init hizPass
        glGenFramebuffers(1, &hizPass);

        //init visibility map
        glGenTextures(1, &visibilityMap);
        glBindTexture(GL_TEXTURE_2D, visibilityMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelsCount - 1);
        glGenerateMipmap(GL_TEXTURE_2D);

        //init pre-convolution Pass
        glGenFramebuffers(1, &pre_convolutionPass);
        glGenTextures(1, &tempTex);
        glBindTexture(GL_TEXTURE_2D, tempTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levelsCount - 1);
        glGenerateMipmap(GL_TEXTURE_2D);

        //init shadow pass
        glGenFramebuffers(1, &shadowPass);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowPass);
        glGenTextures(1, &shadow);
        glBindTexture(GL_TEXTURE_2D, shadow);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow, 0);
        glDrawBuffers(1, attachments);

        glGenFramebuffers(1, &blur1Pass);
        glBindFramebuffer(GL_FRAMEBUFFER, blur1Pass);
        genTexture(&SSABSS_Blur1_1);
        genTexture(&SSABSS_Blur1_2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSABSS_Blur1_1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, SSABSS_Blur1_1, 0);
        glDrawBuffers(2, attachments);

        glGenFramebuffers(1, &blur2Pass);
        glBindFramebuffer(GL_FRAMEBUFFER, blur2Pass);
        genTexture(&SSABSS_Blur2_1);
        genTexture(&SSABSS_Blur2_2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSABSS_Blur2_1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, SSABSS_Blur2_2, 0);
        glDrawBuffers(2, attachments);

        glGenFramebuffers(1, &blur3Pass);
        glBindFramebuffer(GL_FRAMEBUFFER, blur3Pass);
        genTexture(&SSABSS_Blur3_1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSABSS_Blur3_1, 0);
        glDrawBuffers(1, attachments);

        glGenFramebuffers(1, &preCalPass);
        glBindFramebuffer(GL_FRAMEBUFFER, preCalPass);
        genTexture(&SSABSS_PreCal_1);
        genTexture(&SSABSS_PreCal_2);
        genTexture(&SSABSS_PreCal_3);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSABSS_PreCal_1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, SSABSS_PreCal_2, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, SSABSS_PreCal_3, 0);
        glDrawBuffers(3, attachments);

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

            genGbuffer();

            directLighting();

            genHizbuffer();

            pre_convolution();

            ssr();

            glfwSwapBuffers(window);
            glfwPollEvents();
//            std::cout << deltaTime << std::endl;
        }
    }

    void SOpenGL::genTexture(unsigned int *textureID)
    {
        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void SOpenGL::genGbuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferPass);
        glClearColor(0.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gbuffer_shader.use();
        glm::mat4 projection = mainCamera.get_Projection(WIDTH, HEIGHT, false);
        glm::mat4 view = mainCamera.get_ViewMatrix();
        gbuffer_shader.setMat4("projection", projection);
        gbuffer_shader.setMat4("view", view);
        set_light();
        //gbuffer_shader.setMat4("model", glm::mat4(1.0f));
        //scene_root[1]->draw(gbuffer_shader);
//        for (auto& object : scene_root)
//        {
//            //std::cout << "Draw" << std::endl;
//            object->draw(gbuffer_shader);
//        }
        scene_root[0]->draw(gbuffer_shader);
        scene_root[1]->draw(gbuffer_shader);

        // draw shadow map
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapPass);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shadowMap_shader.use();
        projection = lightCamera.get_Projection(WIDTH, HEIGHT, true);
        view = lightCamera.get_ViewMatrix();
        lightSpaceMatrix = projection * view;
        shadowMap_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//        for (auto& object : scene_root)
//        {
//            //std::cout << "Draw" << std::endl;
//            object->draw(shadowMap_shader);
//        }
        scene_root[0]->draw(shadowMap_shader);
        scene_root[1]->draw(shadowMap_shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SOpenGL::directLighting()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, directPass);
        direct_shader.use();
        direct_shader.setVec3("lightPos", lights[0].get_position());
        direct_shader.setVec3("lightColor", lights[0].get_color());
        direct_shader.setVec3("cameraPos", mainCamera.get_Position());
        direct_shader.setMat4("inverseProj", mainCamera.get_invProjection(WIDTH, HEIGHT));
        direct_shader.setMat4("inverseView", mainCamera.get_invView());
        direct_shader.setMat4("view", mainCamera.get_ViewMatrix());
        direct_shader.setInt("BaseColorMap", 0);
        direct_shader.setInt("NormalMap", 1);
        direct_shader.setInt("DepthMap", 2);
        direct_shader.setInt("PositionMap", 3);

        for (int i = 0; i < 3; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, GBuffer[i]);
        }
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, worldPosition);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

    }

    void SOpenGL::genHizbuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, hizPass);
        glDepthMask(GL_FALSE);
        int lastWidth = WIDTH;
        int lastHeight = HEIGHT;

        hiz_shader.use();
        hiz_shader.setInt("Depthmap", 0);
        hiz_shader.setInt("VisibilityMap", 1);
        // bind depth buffer
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GBuffer[2]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, visibilityMap);
        unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        hiz_shader.setInt("previousLevel", -1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GBuffer[2], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, visibilityMap, 0);
        glDrawBuffers(2, attachments);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        for (int i = 1; i < levelsCount; i++)
        {
            //glClearColor(0.0, 0.0, 0.0, 1.0);
            //glClear(GL_COLOR_BUFFER_BIT);
            hiz_shader.setVec2i("previousDim", glm::ivec2(lastWidth, lastHeight));
            hiz_shader.setInt("previousLevel", i - 1);
            lastWidth /= 2;
            lastHeight /= 2;
            lastWidth = lastWidth > 0 ? lastWidth : 1;
            lastHeight = lastHeight > 0 ? lastHeight : 1;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GBuffer[2], i);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, visibilityMap, i);
            glDrawBuffers(2, attachments);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

        }
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SOpenGL::pre_convolution()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pre_convolutionPass);
        glDepthMask(GL_FALSE);
        int lastWidth = WIDTH;
        int lastHeight = HEIGHT;

        blur_shader.use();
        blur_shader.setInt("ColorBuffer", 0);

        for (int i = 1; i < levelsCount; i++)
        {
            // bind Color buffer
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, directResult);
            blur_shader.setInt("previousLevel", i - 1);
            blur_shader.setBool("isVertical", true);
            lastWidth /= 2;
            lastHeight /= 2;
            lastWidth = lastWidth > 0 ? lastWidth : 1;
            lastHeight = lastHeight > 0 ? lastHeight : 1;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTex, i);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tempTex);
            blur_shader.setInt("previousLevel", i - 1);
            blur_shader.setBool("isVertical", false);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, directResult, i);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTex, 0);
        }
        glDepthMask(GL_TRUE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void SOpenGL::calculateShadow()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, preCalPass);
        preCal_shader.use();
    }

    void SOpenGL::ssr()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightViewMat = lightCamera.get_ViewMatrix();
        glm::mat4 lightProjMat = lightCamera.get_Projection(WIDTH, HEIGHT, true);
        glm::mat4 lightVP = lightProjMat * lightViewMat;
        ssr_shader.use();
        ssr_shader.setInt("ColorBuffer", 0);
        ssr_shader.setInt("NormalBuffer", 1);
        ssr_shader.setInt("DepthBuffer", 2);
        ssr_shader.setInt("PositionBuffer", 3);
        ssr_shader.setInt("VisibilityBuffer", 4);
        ssr_shader.setInt("ShadowMap", 5);
        ssr_shader.setMat4("inverseProj", mainCamera.get_invProjection(WIDTH, HEIGHT));
        ssr_shader.setMat4("inverseView", mainCamera.get_invView());
        ssr_shader.setMat4("view", mainCamera.get_ViewMatrix());
        ssr_shader.setMat4("projection", mainCamera.get_Projection(WIDTH, HEIGHT, false));
        ssr_shader.setMat4("lightVP", lightVP);
        ssr_shader.setVec3("uLightPos", lightCamera.get_Position());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, directResult);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, GBuffer[1]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, GBuffer[2]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, worldPosition);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, visibilityMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
//        glActiveTexture(GL_TEXTURE0 + 3);
//        glBindTexture(GL_TEXTURE_2D, shadowMap);
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

//    void SOpenGL::genShadow()
//    {
//        glm::mat4 lightViewMat = lightCamera.get_ViewMatrix();
//        glm::mat4 lightProjMat = lightCamera.get_Projection(WIDTH, HEIGHT, true);
//        glm::mat4 lightVP = lightProjMat * lightViewMat;
//        glBindFramebuffer(GL_FRAMEBUFFER, shadowPass);
//        glClearColor(1.0, 1.0, 1.0, 1.0);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        shadow_shader.use();
//        shadow_shader.setInt("ShadowMap", 0);
//        shadow_shader.setInt("DepthMap", 1);
//        shadow_shader.setInt("NormalBuffer", 2);
//        shadow_shader.setVec3("uLightPos", lights[0].get_position());
//        shadow_shader.setMat4("lightVP", lightVP);
//        shadow_shader.setMat4("inverseProj", mainCamera.get_invProjection(WIDTH, HEIGHT));
//        shadow_shader.setMat4("inverseView", mainCamera.get_invView());
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, shadowMap);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, GBuffer[2]);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, GBuffer[1]);
//        glBindVertexArray(quadVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        glBindVertexArray(0);
//    }

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
            //glClearColor(1.0, 1.0, 1.0, 1.0);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gbuffer_shader.use();
            glm::mat4 projection = mainCamera.get_Projection(WIDTH, HEIGHT, false);
            gbuffer_shader.setMat4("projection", projection);
            gbuffer_shader.setMat4("view", mainCamera.get_ViewMatrix());
            gbuffer_shader.setVec3("cameraPos", mainCamera.get_Position());
            set_light();
            //gbuffer_shader.setMat4("model", glm::mat4(1.0f));
            //scene_root[1]->draw(gbuffer_shader);
            for (auto& object : scene_root)
            {
                //std::cout << "Draw" << std::endl;
                object->draw(gbuffer_shader);
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
