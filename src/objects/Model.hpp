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
            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode* node, const aiScene* scene)
        {
            //TODO
        }
    };
}
#endif //SIMPLE_RENDERER_MODEL_HPP
