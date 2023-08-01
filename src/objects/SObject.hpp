#ifndef SIMPLE_RENDERER_SOBJECT_HPP
#define SIMPLE_RENDERER_SOBJECT_HPP
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "util/Shader.h"

enum class TextureType
{
    NORMAL,
    DIFFUSE,
    HEIGHT,
    SPECULAR,
    METALLIC,
    ROUGHNESS,
    AO
};

struct Texture
{
    unsigned int id;
    TextureType type;
    std::string path;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
    glm::vec3 bitangent;
};

class SObject
{
private:
    std::shared_ptr<SObject> child;
    std::shared_ptr<SObject> parent;
    //bool hasNormalMap;
protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::vector<Texture> textures;
    glm::vec3 albedo;
    float metallic;
    float roughness;
    float ao;
public:
    virtual void Draw(SRenderer::Shader& shader) = 0;

    SObject() : position(0.0, 0.0, 0.0), rotation(0.0, 0.0, 0.0), scale(1.0, 1.0, 1.0), textures(), albedo(1.0, 1.0, 1.0), metallic(0), roughness(0), ao(0)
    {
    }

    SObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) : position(pos), rotation(rot), scale(scale), textures(), albedo(1.0, 1.0, 1.0), metallic(0), roughness(0), ao(0)
    {
    }

    void add_child(std::shared_ptr<SObject> c)
    {
        child = c;
    }

    void add_parent(std::shared_ptr<SObject> p)
    {
        parent = p;
    }

    void set_position(const glm::vec3 pos)
    {
        position = pos;
    }
    glm::vec3 get_position() const
    {
        return position;
    }

    void set_rotation(const glm::vec3 rot)
    {
        rotation = rot;
    }
    glm::vec3 get_rotation() const
    {
        return rotation;
    }

    void set_scale(const glm::vec3 sc)
    {
        scale = sc;
    }
    glm::vec3 get_scale() const
    {
        return scale;
    }

    void set_albedo(const glm::vec3 col)
    {
        albedo = col;
    }
    glm::vec3 get_color() const
    {
        return albedo;
    }

    void set_metallic(const float m)
    {
        metallic = m;
    }
    float get_metallic() const
    {
        return metallic;
    }

    void set_roughness(const float r)
    {
        roughness = r;
    }
    float get_roughness() const
    {
        return roughness;
    }

    void set_ao(const float ao)
    {
        this->ao = ao;
    }
    float get_ao() const
    {
        return ao;
    }

    virtual void add_texture(std::string path, TextureType type)
    {
        unsigned int tex_id;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, channel;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
        if (data != nullptr)
        {
            GLenum format = GL_RED;
            if (channel == 1)
            {
                format = GL_RED;
            }
            else if (channel == 3)
            {
                format = GL_RGB;
            }
            else if (channel == 4)
            {
                format = GL_RGBA;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            textures.push_back(Texture{tex_id, type, path});
        }
        else
        {
            std::cerr << "Load image error!" << std::endl;
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
#endif //SIMPLE_RENDERER_SOBJECT_HPP
