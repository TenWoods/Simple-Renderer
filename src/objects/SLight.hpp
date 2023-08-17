#ifndef SIMPLE_RENDERER_SLIGHT_HPP
#define SIMPLE_RENDERER_SLIGHT_HPP
#include <glm/vec3.hpp>

namespace SRenderer
{
    enum class LightType
    {
        SPOT,
        POINT,
        DIRECTIONAL
    };
    class SLight
    {
    private:
        glm::vec3 position;
        glm::vec3 color;
        LightType type;
    public:
        SLight() : position(0.0, 0.0, 0.0), color(1.0, 1.0, 1.0), type(LightType::DIRECTIONAL)
        {

        }
        SLight(glm::vec3 pos, glm::vec3 col) : position(pos), color(col), type(LightType::DIRECTIONAL)
        {

        }

        void set_position(float x, float y, float z)
        {
            position = glm::vec3(x, y, z);
        }
        void set_position(glm::vec3 pos)
        {
            position = pos;
        }
        void set_color(float r, float g, float b)
        {
            color = glm::vec3(r, g, b);
        }
        void set_color(glm::vec3 col)
        {
            color = col;
        }
        glm::vec3 get_position() const
        {
            return position;
        }
        glm::vec3 get_color() const
        {
            return color;
        }
    };

}

#endif //SIMPLE_RENDERER_SLIGHT_HPP
