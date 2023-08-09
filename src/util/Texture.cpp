#include "Texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../util/stb_image.hpp"
namespace SRenderer
{
    Texture::Texture(std::string path, SRenderer::TextureType t) : type(t), ID(-1)
    {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        uint8_t* data = stbi_load(path.c_str(), &width, &height, &component, 0);
        if (data != nullptr)
        {
            GLenum format = GL_RED;
            if (component == 1)
            {
                format = GL_RED;
            }
            else if (component == 3)
            {
                format = GL_RGB;
            }
            else if (component == 4)
            {
                format = GL_RGBA;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            this->path = path;
        }
        else
        {
            std::cerr << "Load image error!" << std::endl;
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}