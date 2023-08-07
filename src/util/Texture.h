#ifndef SIMPLE_RENDERER_TEXTURE_H
#define SIMPLE_RENDERER_TEXTURE_H
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "../util/stb_image.hpp"
#ifndef __glad_h_
#include <glad/glad.h>
#endif

namespace SRenderer
{
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

    class Texture
    {
    public:
        uint32_t ID;
        int width;
        int height;
        int component;
        TextureType type;
    public:
        Texture(std::string path, TextureType type);
    };

}


#endif //SIMPLE_RENDERER_TEXTURE_H
