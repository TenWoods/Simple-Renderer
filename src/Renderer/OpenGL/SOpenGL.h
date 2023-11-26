#ifndef SIMPLE_RENDERER_SOPENGL_H
#define SIMPLE_RENDERER_SOPENGL_H
#include "../../util/Shader.h"
#include <GLFW/glfw3.h>
#include <memory>
#include "../../objects/Model.hpp"
#include "../../objects/SLight.hpp"
#include "../../util/Camera.h"

namespace SRenderer
{
    class SOpenGL
    {
    private:
        GLFWwindow* window;
        const int WIDTH = 1024;
        const int HEIGHT = 1024;
        std::vector<std::shared_ptr<SObject>> scene_root;
        Shader gbuffer_shader;
        Shader direct_shader;
        Shader ssr_shader;
        Shader hiz_shader;
        Shader blur_shader;
        Shader shadow_shader;
        Camera mainCamera;
        Camera lightCamera;
        glm::mat4 lightSpaceMatrix;
        std::vector<SLight> lights;
        float lastFrame;
        float deltaTime;
        float lastX;
        float lastY;
        bool firstMouse;

        unsigned int gbufferPass;
        unsigned int shadowMapPass;
        unsigned int directPass;
        unsigned int hizPass;
        unsigned int ssrPass;
        unsigned int pre_convolutionPass;
        unsigned int shadowPass;

        unsigned int shadowMap;
        unsigned int shadow;
        unsigned int directResult, viewPosition;
        unsigned int tempTex;
        unsigned int ssrResult;
        unsigned int quadVAO, quadVBO;
        unsigned int GBuffer[3];
        unsigned int worldPosition;
        int levelsCount;
    private:
        void initWindow();
        void renderLoop();
        void release();
        void processInput(GLFWwindow* window);
        void set_light();

        void forwardRendering();
        void deferredRendering();

        void genGbuffer();
        void directLighting();
        void genHizbuffer();
        void ssr();
        void pre_convolution();
        void postRendering();
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
