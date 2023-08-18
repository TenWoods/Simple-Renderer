#ifndef SIMPLE_RENDERER_SOBJECT_HPP
#define SIMPLE_RENDERER_SOBJECT_HPP
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>
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
        glm::vec4 base_color;
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
    public:
        virtual void draw(const Shader& shader)
        {

        }

        SObject() : position(0.0, 0.0, 0.0), rotation(0.0, 0.0, 0.0), scale(1.0, 1.0, 1.0)
        {
        }

        SObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) : position(pos), rotation(rot), scale(scale)
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
    };
}
#endif //SIMPLE_RENDERER_SOBJECT_HPP
