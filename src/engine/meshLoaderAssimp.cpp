#include <cassert>
#include <optional>
#include <string_view>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <projectV/core/mesh.hpp>

#include <projectV/engine/resource/meshLoaderAssimp.hpp>
#include <projectV/engine/logging/log.hpp>

namespace projectv::engine
{
    unsigned int MeshLoaderAssimp::initPostProcessFlags(bool bFlipUV)
    {
        unsigned int outputFlag = 0;

        outputFlag |= aiProcess_Triangulate;
        outputFlag |= aiProcess_JoinIdenticalVertices;

        // Normals / tangents if you need them
        outputFlag |= aiProcess_GenNormals;
        // flags |= aiProcess_CalcTangentSpace; // later if needed

        // Coordinate system conversions
        outputFlag |= aiProcess_MakeLeftHanded;
        outputFlag |= aiProcess_FlipWindingOrder;

        if(bFlipUV) outputFlag |= aiProcess_FlipUVs;

        return outputFlag;
    }

    std::optional<projectv::core::Mesh> MeshLoaderAssimp::buildMeshFromScene(const aiScene *scene, const projectv::core::MeshLoadParams &params)
    {
        assert(scene);
        assert(scene->mNumMeshes > 0);

        aiMesh* srcMesh = scene->mMeshes[0];

        if (!validateMesh(srcMesh)) return std::nullopt;

        projectv::core::Mesh mesh;
        mesh.vertices.resize(srcMesh->mNumVertices);

        const bool hasNormals   = srcMesh->HasNormals();
        const bool hasTexCoords = srcMesh->HasTextureCoords(0); // first UV channel

        for (unsigned int i = 0; i < srcMesh->mNumVertices; ++i)
        {
            projectv::core::Vertex& dst = mesh.vertices[i];

            // Positions
            const aiVector3D& p = srcMesh->mVertices[i];
            dst.position.x = p.x;
            dst.position.y = p.y;
            dst.position.z = p.z;

            // Normals
            if (hasNormals)
            {
                const aiVector3D& n = srcMesh->mNormals[i];
                dst.normal.x = n.x;
                dst.normal.y = n.y;
                dst.normal.z = n.z;
            }

            // UV0 – Assimp stores as 3D (xyz), we use xy
            if (hasTexCoords)
            {
                const aiVector3D& t = srcMesh->mTextureCoords[0][i];
                dst.uv.x = t.x;
                dst.uv.y = t.y;
            }
        }

        // Build index buffer (faces are triangles due to aiProcess_Triangulate)
        mesh.indices.clear();
        mesh.indices.reserve(srcMesh->mNumFaces * 3);

        for (unsigned int i = 0; i < srcMesh->mNumFaces; ++i)
        {
            const aiFace& face = srcMesh->mFaces[i];
            if (face.mNumIndices != 3)
            {
                // Should not happen with Triangulate, but be safe
                projectv::engine::LogError(
                    "Non-triangle face ({}) in mesh '{}', skipping.");
                continue;
            }

            mesh.indices.push_back(static_cast<uint32_t>(face.mIndices[0]));
            mesh.indices.push_back(static_cast<uint32_t>(face.mIndices[1]));
            mesh.indices.push_back(static_cast<uint32_t>(face.mIndices[2]));
        }

        return mesh;
    }

    bool MeshLoaderAssimp::validateScene(const aiScene* scene, const Assimp::Importer& importer)
    {
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            projectv::engine::LogError(importer.GetErrorString());
            return false;
        }

        if (scene->mNumMeshes == 0)
        {
            projectv::engine::LogError("Assimp scene '{}' contains no meshes.");
            return false;
        }

        return true;
    }

    bool MeshLoaderAssimp::validateMesh(const aiMesh* mesh)
    {
        if (!mesh || mesh->mNumVertices == 0 || mesh->mNumFaces == 0)
        {
            projectv::engine::LogError("Assimp mesh '{}' has no vertices or faces.");
            return false;
        }

        return true;
    }

    std::optional<projectv::core::Mesh> MeshLoaderAssimp::loadMesh(const projectv::core::MeshLoadParams &params)
    {
        Assimp::Importer importer;

        if(params.path.empty())
        {
            projectv::engine::LogError("Assimp failed, empty file path sent");
        }

        const aiScene* scene = importer.ReadFile(
            std::string(params.path).c_str(),
            initPostProcessFlags(params.bFlipUVs));

        if(!validateScene(scene, importer)) return std::nullopt;
            
        return buildMeshFromScene(scene, params);
    }
}