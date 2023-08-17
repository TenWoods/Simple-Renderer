cmake_minimum_required(VERSION 3.23)
project(Simple_Renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/libs)

if(UNIX)
    link_directories(${PROJECT_SOURCE_DIR}/libs/linux)
    include_directories(3rd/glm/glm)
elseif(WIN32)
    link_directories(${PROJECT_SOURCE_DIR}/libs/win)
    if (NOT DEFINED ENV{VK_SDK_PATH})
        message(FATAL_ERROR "VK_SDK_PATH not defined!")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"MSVCRT.lib")
    set(VK_SDK_PATH $ENV{VK_SDK_PATH})
    set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
    set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)
    find_package(Vulkan REQUIRED)
    include_directories(${VK_SDK_INCLUDE})
endif()
find_package(Assimp REQUIRED)

#set(LIBRARY_DIR ${VCPKG_ROOT}/installed/x64-windows/lib)
#set(INCLUDE_DIR ${VCPKG_ROOT}/installed/x64-windows/include)
#link_directories(${LIBRARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/3rd/glfw/include
        ${PROJECT_SOURCE_DIR}/3rd/imgui
        ${PROJECT_SOURCE_DIR}/3rd/imgui/backends
        ${PROJECT_SOURCE_DIR}/3rd/glad/include)

aux_source_directory(src/util UTIL)
aux_source_directory(src/objects OBJ)

file(GLOB IMGUI 3rd/imgui/*.cpp 3rd/imgui/backends/imgui_impl_glfw.cpp 3rd/imgui/backends/imgui_impl_opengl3.cpp)

add_executable(Simple_Renderer src/OpenGL_test.cpp src/glad.c src/Renderer/OpenGL/SOpenGL.cpp ${UTIL} ${OBJ})
target_link_libraries(Simple_Renderer PRIVATE assimp::assimp glfw3)

add_executable(Simple_Renderer_vulkan src/vulkan_test.cpp src/Renderer/Vulkan/SVulkan.cpp ${OBJ})
target_link_libraries(Simple_Renderer_vulkan glfw3 assimp::assimp Vulkan::Vulkan)
