#ifndef SIMPLE_RENDERER_MODEL_HPP
#define SIMPLE_RENDERER_MODEL_HPP
#include "SObject.hpp"
#include "Mesh.hpp"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace SRenderer
{
    class Model : public SObject
    {
    private:
        std::vector<Mesh> meshes;
        std::vector<Texture> loaded_textures;
        std::string directory;
    public:
        Model(const std::string& path) : meshes(), loaded_textures()
        {
            loadModel(path);

        }

        void draw(const Shader& shader) override
        {
            //std::cout << "Draw!" << std::endl;
            //TODO: set position, rotation and scale
            shader.use();
            glm::mat4 model(1.0f);
            model = glm::translate(model, position);
            model = glm::scale(model, scale);
            shader.setMat4("model", model);
            for (const auto& mesh : meshes)
            {
                mesh.draw(shader);
            }
            //std::cout << std::endl << std::endl;
        }

    private:
        void loadModel(const std::string& path)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path,
                                                     aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenNormals);
            if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
            {
                std::cerr << "Load model failed: " << importer.GetErrorString() << std::endl;
                return;
            }
            directory = path.substr(0, path.find_last_of('/')) +'/';
            //std::cout << path << ' ' << directory << std::endl;
            processNode(scene->mRootNode, scene);
            std::cout << "Load model: <" << path << "> success" << std::endl;
        }

        void processNode(aiNode* node, const aiScene* scene)
        {
            //std::cout << "Mesh Num: " << node->mNumMeshes << std::endl;
            for (int i = 0; i < node->mNumMeshes; i++)
            {
                //std::cout << "Process Mesh " << i << std::endl;
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.emplace_back(processMesh(mesh, scene));
            }
            //std::cout << "Children Num: " << node->mNumChildren << std::endl;
            for (int i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh processMesh(aiMesh* mesh, const aiScene* scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            //std::cout << '\t' << "Vertex Num: " << mesh->mNumVertices << std::endl;
            for (int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex{};
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                if (mesh->mTangents != nullptr)
                {
                    //std::cout << "Tangent" << std::endl;
                    vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                }
                //std::cout << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z << std::endl;
                if (mesh->mTextureCoords[0] != nullptr)
                {
                    vertex.texcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertex.texcoord = glm::vec2(0.0f, 0.0f);
                }
                vertices.push_back(vertex);
            }
            //std::cout << '\t' << "Face Num: " << mesh->mNumFaces << std::endl;
            for (int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                //std::cout << "Face" << i << "indices Num: " << face.mNumIndices << std::endl;
                for (int j = 0; j < face.mNumIndices; j++)
                {
                    indices.emplace_back(face.mIndices[j]);
                }
            }
            //std::cout << '\t' << "Material Index: " << mesh->mMaterialIndex << std::endl;
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> baseColors = loadMaterialTextures(material, aiTextureType_BASE_COLOR, TextureType::BASE_COLOR);
                textures.insert(textures.end(), baseColors.begin(), baseColors.end());
                std::vector<Texture> normalMap = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NORMAL);
                textures.insert(textures.end(), normalMap.begin(), normalMap.end());
                std::vector<Texture> metallicRoughnessMap = loadMaterialTextures(material, aiTextureType_UNKNOWN, TextureType::METALLIC_ROUGHNESS);
                textures.insert(textures.end(), metallicRoughnessMap.begin(), metallicRoughnessMap.end());
                std::vector<Texture> aoMap = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AO);
                textures.insert(textures.end(), aoMap.begin(), aoMap.end());
            }
            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType aiType, TextureType type)
        {
            std::vector<Texture> textures;
            //std::cout << '\t' << "Texture Num: " << material->GetTextureCount(aiType) << std::endl;
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
