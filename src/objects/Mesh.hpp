#ifndef SIMPLE_RENDERER_MESH_HPP
#define SIMPLE_RENDERER_MESH_HPP
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include "../util/Shader.h"
#include "SObject.hpp"

namespace SRenderer
{
    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
    private:
        unsigned int VAO, VBO, EBO;
    private:
        void setUpMesh()
        {
            //std::cout << '\t' << "Set Up Mesh" << std::endl;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
            glBindVertexArray(0);
        }
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        {
            this->vertices = std::move(vertices);
            this->indices = std::move(indices);
            this->textures = std::move(textures);
            setUpMesh();
        }

        void draw(const Shader& shader) const
        {
            shader.use();
            //std::cout << "Draw Mesh ";
            glBindVertexArray(VAO);
            int index = 0;
            for (const auto& texture : textures)
            {
                switch (texture.type)
                {
                    case TextureType::BASE_COLOR:
                        shader.setInt("BaseColorMap", index);
                        break;
                    case TextureType::NORMAL:
                        shader.setInt("NormalMap", index);
                        break;
                    case TextureType::METALLIC_ROUGHNESS:
                        shader.setInt("MetallicRoughnessMap", index);
                        break;
                    case TextureType::AO:
                        shader.setInt("AOMap", index);
                        break;
                    default:
                        shader.setInt("OtherMap", index);
                        break;
                }
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, texture.ID);
                index++;
            }
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(0);
        }
    };
}
#endif //SIMPLE_RENDERER_MESH_HPP
