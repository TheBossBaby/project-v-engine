#pragma once

#include <projectV/core/meshLoader.hpp>

namespace Assimp{
    class  Importer;
}

struct aiMesh;
struct aiScene;

namespace projectv::engine
{
    /**
     * @brief Mesh loader implementation based on Assimp.
     *
     * This class converts mesh data loaded by Assimp into the engine's
     * Mesh format. It currently assumes that each model file contains
     * exactly one mesh and only the first mesh in the scene is used.
     *
     * @note Only the first mesh in the imported scene is processed.
     *       Files containing multiple meshes or full scene hierarchies
     *       are not yet supported.
     */
    class MeshLoaderAssimp : public projectv::core::IMeshLoader
    {
        private:
            /**
             * @brief Initialize Assimp post‑process flags.
             *
             * Builds the set of Assimp post‑processing flags that enforce
             * engine conventions (triangulation, left‑handed coordinates,
             * front‑face winding, etc.) and optionally flips UVs.
             *
             * @param bFlipUV
             *        If true, adds the Assimp flag to flip UV coordinates
             *        vertically during import.
             *
             * @return A bitmask of Assimp post‑processing flags.
             */
            unsigned int initPostProcessFlags(bool bFlipUV);

            /**
             * @brief Build an engine Mesh from an Assimp scene.
             *
             * Uses the first mesh in the given Assimp scene and converts
             * its vertices and indices into a projectv::core::Mesh.
             *
             * @param scene
             *        Valid Assimp scene returned by the importer.
             * @param params
             *        Mesh loading parameters used for this import.
             *
             * @return A populated Mesh on success; otherwise std::nullopt.
             *
             * @note Only the first mesh in @p scene is processed. Additional
             *       meshes and node hierarchies are currently ignored.
             */
            std::optional<projectv::core::Mesh> buildMeshFromScene(const aiScene* scene, const projectv::core::MeshLoadParams& params);

            /**
             * @brief Validate the imported Assimp scene.
             *
             * Checks that the scene is non‑null, not marked incomplete,
             * has a root node, and contains at least one mesh. Logs an
             * error message if validation fails.
             *
             * @param scene
             *        Scene pointer returned by Assimp::Importer::ReadFile.
             * @param importer
             *        Importer instance used to load the scene; provides
             *        detailed error messages.
             *
             * @return true if the scene is valid; otherwise false.
             */
            bool validateScene(const aiScene* scene, const Assimp::Importer& importer);

            /**
             * @brief Validate a single Assimp mesh.
             *
             * Ensures that the mesh pointer is non‑null and that it
             * contains at least one vertex and one face. Logs an error
             * message if validation fails.
             *
             * @param mesh
             *        Mesh pointer taken from the Assimp scene.
             *
             * @return true if the mesh is valid; otherwise false.
             */
            bool validateMesh(const aiMesh* srcMesh);
        public:
            /**
             * @brief Load a mesh using Assimp.
             *
             * Imports the mesh specified in @p params using Assimp,
             * applies engine‑specific post‑processing, and converts
             * the first mesh in the scene into a projectv::core::Mesh.
             *
             * @param params
             *        Mesh loading parameters, including source path and
             *        import flags (UV flipping, centering, normalization).
             *
             * @return The loaded Mesh on success; otherwise std::nullopt.
             *
             * @note Currently supports only a single mesh per file and
             *       processes only the first mesh in the imported scene.
             */
            std::optional<projectv::core::Mesh> loadMesh(const projectv::core::MeshLoadParams& params) override;
    };
}