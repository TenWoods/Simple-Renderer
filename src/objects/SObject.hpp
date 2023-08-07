#ifndef SIMPLE_RENDERER_SOBJECT_HPP
#define SIMPLE_RENDERER_SOBJECT_HPP
#include <glm.hpp>
#include <string>
#include <memory>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../util/stb_image.hpp"
#include "../util/Shader.h"
#include "../util/Texture.h"

namespace SRenderer
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texcoord;
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
            Texture tmp = Texture(path, type);
            if (tmp.ID == -1)
            {
                std::cerr << path << ": load error" << std::endl;
            }
            else
            {
                textures.emplace_back(Texture(path, type));
            }
        }
    };
}
#endif //SIMPLE_RENDERER_SOBJECT_HPP
