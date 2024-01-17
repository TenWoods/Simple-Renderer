#ifndef SIMPLE_RENDERER_SOPENGL_H
#define SIMPLE_RENDERER_SOPENGL_H
#include "../../util/Shader.h"
#include <GLFW/glfw3.h>
#include <memory>
#include "../../objects/Model.hpp"
#include "../../objects/SLight.hpp"
#include "../../util/Camera.h"
#include "../../util/logger.hpp"
namespace SRenderer
{
    class SOpenGL
    {
    private:
        logger m_logger;
        GLFWwindow* window;
        const int WIDTH = 1024;
        const int HEIGHT = 1024;
        std::vector<std::shared_ptr<SObject>> scene_root;
        Shader gbuffer_shader;
        Shader direct_shader;
        Shader ssr_shader;
        Shader hiz_shader;
        Shader blur_shader;
        Shader shadowMap_shader;
        Shader blur1_shader;
        Shader blur2_shader;
        Shader blur3_shader;
        Shader preCal_shader;
        Camera mainCamera;
        Camera lightCamera;
        glm::mat4 lightSpaceMatrix;
        std::vector<SLight> lights;
        float lastFrame;
        float deltaTime;
        float lastX;
        float lastY;
        bool firstMouse;
        //FBO
        unsigned int gbufferPass;
        unsigned int shadowMapPass;
        unsigned int directPass;
        unsigned int hizPass;
        unsigned int pre_convolutionPass;
//        unsigned int shadowPass;
        unsigned int blur1Pass;
        unsigned int blur2Pass;
        unsigned int blur3Pass;
        unsigned int preCalPass;
        //Textures
        unsigned int shadowMap;
        unsigned int directResult, viewPosition;
        unsigned int tempTex;
        unsigned int visibilityMap;
        unsigned int quadVAO, quadVBO;
        unsigned int GBuffer[3];
        unsigned int worldPosition;
        unsigned int SSABSS_Blur1_1, SSABSS_Blur1_2, SSABSS_Blur2_1, SSABSS_Blur2_2, SSABSS_Blur3_1;
        unsigned int SSABSS_PreCal_1, SSABSS_PreCal_2, SSABSS_PreCal_3;
        int levelsCount;
    private:
        void initWindow();
        void renderLoop();
        void release();
        void processInput(GLFWwindow* window);
        void set_light();

        void forwardRendering();
        void deferredRendering();

        void genTexture(unsigned int *textureID);

        void genGbuffer();
        void directLighting();
        void genHizbuffer();
        void calculateShadow();
        void ssr();
        void pre_convolution();
    private:
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    public:
        SOpenGL();
        void run();
        void addModel(std::string path);
        void addLight(SLight light);

    };
}



#endif //SIMPLE_RENDERER_SOPENGL_H
