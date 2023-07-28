cmake_minimum_required(VERSION 3.23)
project(Simple_Renderer)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/3rd/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd/glm)
include_directories(${PROJECT_SOURCE_DIR}/3rd/imgui)
include_directories(${PROJECT_SOURCE_DIR}/3rd/glad/include)

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(Simple_Renderer src/main.cpp src/glad.c src/util/Camera.cpp src/util/Camera.h src/util/Shader.cpp src/util/Shader.h)
target_link_libraries(Simple_Renderer opengl32 glfw3)
