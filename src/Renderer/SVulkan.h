#ifndef SIMPLE_RENDERER_SVULKAN_H
#define SIMPLE_RENDERER_SVULKAN_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
namespace SRenderer
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    const std::vector validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    class SVulkan
    {
    private:
        GLFWwindow* window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
    private:
        void initWindow();
        void initVulkan();
        void createInstance();
        void setupDebugMessenger();
        void renderLoop();
        void release();
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void pickPhysicalDevice();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData)
        {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }
    public:
        void run();
    };

} // SVulkan

#endif //SIMPLE_RENDERER_SVULKAN_H
