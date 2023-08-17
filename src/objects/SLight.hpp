#ifndef SIMPLE_RENDERER_SLIGHT_HPP
#define SIMPLE_RENDERER_SLIGHT_HPP
#include <glm/vec3.hpp>

namespace SRenderer
{
    class SLight
    {
    private:
        glm::vec3 position;
        glm::vec3 color;
    public:
        void set_position(float x, float y, float z);
        void set_position(glm::vec3 pos);
        void set_color(float r, float g, float b);
        void set_color(glm::vec3 col);
    };
}

#endif //SIMPLE_RENDERER_SLIGHT_HPP
