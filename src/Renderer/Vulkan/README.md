# Vulkan Memo
## How Vulkan draws a triangle

1. Instance and physical device selection

   Create a [`VkInstance`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkInstance.html)

   Select a supported graphics card ([`VkPhysicalDevice`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkPhysicalDevice.html))

2. Logical device and queue families

   Create a [`VkDevice`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkDevice.html) and [`VkQueue`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkQueue.html) for drawing and presentation

3. Window surface and swap chain

   Create a window, window surface and swap chain

4. Image views and framebuffers

   Wrap the swap chain images into [`VkImageView`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkImageView.html)

5. Render passes

   Create a render pass that specifies the render targets and usage

   Create framebuffers for the render pass

6. Graphics pipeline

   Set up the graphics pipeline

7. Command pools and command buffers

   Allocate and record a command buffer with the draw commands for every possible swap chain image

8. Main loop

   Draw frames by acquiring images, submitting the right draw command buffer and returning the images back to the swap chain

## What can validation layers do?

- Checking the values of parameters against the specification to detect misuse
- Tracking creation and destruction of objects to find resource leaks
- Checking thread safety by tracking the threads that calls originate from
- Logging every call and its parameters to the standard output
- Tracing Vulkan calls for profiling and replaying