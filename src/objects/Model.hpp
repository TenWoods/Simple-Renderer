#ifndef SIMPLE_RENDERER_MODEL_HPP
#define SIMPLE_RENDERER_MODEL_HPP
#include "SObject.hpp"
#include "Mesh.hpp"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace SRenderer
{
    class Model : public SObject
    {
    private:
        std::vector<Mesh> meshes;
        std::vector<Texture> textures;
        std::vector<Texture> loaded_textures;
        std::string directory;
    public:
        Model(const std::string& path) : meshes(), loaded_textures()
        {
            loadModel(path);
        }

        void draw(const Shader& shader) override
        {
            //TODO: set position, rotation and scale
            for (const auto& mesh : meshes)
            {
                mesh.draw(shader);
            }
        }

    private:
        void loadModel(std::string path)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path.c_str(),
                                                     aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenNormals);
            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
            {
                std::cerr << "Load model failed: " << importer.GetErrorString() << std::endl;
                return;
            }
            directory = path.substr(0, path.find_last_of('/'));
            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode* node, const aiScene* scene)
        {
            for (int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.emplace_back(processMesh(mesh, scene));
            }
        }

        Mesh processMesh(aiMesh* mesh, const aiScene* scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            for (int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex{};
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                if (mesh->mTextureCoords[0] != nullptr)
                {
                    vertex.texcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertex.texcoord = glm::vec2(0.0f, 0.0f);
                }
                vertices.emplace_back(vertex);
            }

            for (int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (int j = 0; j < face.mNumIndices; j++)
                {
                    indices.emplace_back(face.mIndices[j]);
                }
            }

            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> baseColors = loadMaterialTextures(material, aiTextureType_BASE_COLOR, TextureType::BASE_COLOR);
                textures.insert(textures.end(), baseColors.begin(), baseColors.end());
                std::vector<Texture> normalMap = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NORMAL);
                textures.insert(textures.end(), normalMap.begin(), normalMap.end());
                std::vector<Texture> metallicMap = loadMaterialTextures(material, aiTextureType_METALNESS, TextureType::METALLIC);
                textures.insert(textures.end(), metallicMap.begin(), metallicMap.end());
                std::vector<Texture> roughnessMap = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::ROUGHNESS);
                textures.insert(textures.end(), roughnessMap.begin(), roughnessMap.end());
                std::vector<Texture> aoMap = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AO);
                textures.insert(textures.end(), aoMap.begin(), aoMap.end());
            }
            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType aiType, TextureType type)
        {
            std::vector<Texture> textures;
            for (int i = 0; i < material->GetTextureCount(aiType); i++)
            {
                aiString str;
                material->GetTexture(aiType, i, &str);
                bool isExisted = false;
                std::string path = directory + str.C_Str();
                for (const auto& texture : loaded_textures)
                {
                    if (std::strcmp(texture.path.c_str(), path.c_str()) == 0)
                    {
                        textures.emplace_back(texture);
                        isExisted = true;
                        break;
                    }
                }
                if (!isExisted)
                {
                    Texture texture(path.c_str(), type);
                    textures.emplace_back(texture);
                }
            }
            return textures;
        }
    };
}
#endif //SIMPLE_RENDERER_MODEL_HPP
