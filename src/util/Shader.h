#ifndef SIMPLE_RENDERER_SHADER_H
#define SIMPLE_RENDERER_SHADER_H
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace SRenderer
{
    class Shader
    {
    private:
        GLint id;
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        void use() const;
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, glm::vec2 value) const;
        void setVec3(const std::string& name, glm::vec3 value) const;
        void setVec4(const std::string& name, glm::vec4 value) const;
        void setMat2(const std::string& name, glm::mat2 value) const;
        void setMat3(const std::string& name, glm::mat3 value) const;
        void setMat4(const std::string& name, glm::mat4 value) const;
    private:
        void checkCompileError(GLint shaderID, const std::string& type);
    };
}
#endif //SIMPLE_RENDERER_SHADER_H
