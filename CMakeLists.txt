cmake_minimum_required(VERSION 3.23)
project(Simple_Renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/libs)

find_package(assimp CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/3rd/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd/glm)
include_directories(${PROJECT_SOURCE_DIR}/3rd/imgui)
include_directories(${PROJECT_SOURCE_DIR}/3rd/imgui/backends)
include_directories(${PROJECT_SOURCE_DIR}/3rd/glad/include)

aux_source_directory(src/util UTIL)
aux_source_directory(src/objects OBJ)

file(GLOB IMGUI 3rd/imgui/*.cpp 3rd/imgui/backends/imgui_impl_glfw.cpp 3rd/imgui/backends/imgui_impl_opengl3.cpp)

if(UNIX)
    link_directories(${PROJECT_SOURCE_DIR}/libs/linux)
elseif(WIN32)
    link_directories(${PROJECT_SOURCE_DIR}/libs/win)
endif()

add_executable(Simple_Renderer src/main.cpp src/glad.c ${UTIL} ${OBJ} ${IMGUI})
target_link_libraries(Simple_Renderer glfw3 assimp::assimp)

add_executable(Simple_Renderer_vulkan src/main.cpp src/Renderer/SVulkan.cpp)
target_link_libraries(Simple_Renderer glfw3 assimp::assimp Vulkan::Vulkan)
