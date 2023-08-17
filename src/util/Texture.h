#ifndef SIMPLE_RENDERER_TEXTURE_H
#define SIMPLE_RENDERER_TEXTURE_H
#include <string>
#ifndef __glad_h_
#include <glad/glad.h>
#endif

namespace SRenderer
{
    enum class TextureType
    {
        NORMAL,
        BASE_COLOR,
        HEIGHT,
        METALLIC,
        ROUGHNESS,
        AO,
        Other
    };

    class Texture
    {
    public:
        uint32_t ID;
        int width;
        int height;
        int component;
        TextureType type;
        std::string path;
    public:
        Texture(){}
        Texture(std::string path, TextureType type);
        //TODO: Load by vulkan
    };

}


#endif //SIMPLE_RENDERER_TEXTURE_H
