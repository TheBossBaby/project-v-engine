#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <projectV/core/graphicsPipelineHandle.hpp>
#include <projectV/core/gpu/gpuMeshHandle.hpp>

namespace projectv::core
{
    class IRenderer;

    struct LevelDesc;
    struct EntityDesc;
    struct StaticMeshDesc;
    struct SkyboxDesc;
    struct PipelineDesc;
}

namespace projectv::engine
{
    class ShaderManager;
    class MeshManager;
    class World;

    /**
     * @brief Builds the entities of a level from a LevelDesc.
     *
     * The loader only does the mechanical work: it loads the shaders and
     * meshes named in the description, asks the renderer for pipelines and
     * GPU meshes, and spawns entities in the World. It keeps caches so a
     * pipeline or mesh that is used more than once is created only once.
     * It never knows which level it loads; the content comes from the
     * LevelDesc passed to load().
     *
     * Asset paths in the description are relative to ASSETROOT (see
     * util/assets.hpp). One loader can load several levels in a row.
     */
    class LevelLoader
    {
    public:
        /**
         * @brief Creates a loader that fills @p inWorld.
         * @param inWorld         World that receives the spawned entities.
         * @param inMeshManager   Loads and owns the CPU meshes.
         * @param inShaderManager Loads and owns the shaders.
         *
         * The three references are borrowed and must outlive the loader.
         */
        LevelLoader(World &inWorld, MeshManager &inMeshManager, ShaderManager &inShaderManager);

        /**
         * @brief Loads a level into the world.
         *
         * Creates every pipeline in LevelDesc::pipelineList first, then
         * spawns every entity in LevelDesc::entityList. An entry that fails
         * (bad path, missing file, unknown pipeline name, renderer failure)
         * is logged with its name and skipped; the rest of the level still
         * loads. An entity without a transform gets the default (identity)
         * Transform. Skybox entities are not implemented yet and are skipped
         * with a warning.
         *
         * @param inOutRenderer      Renderer that creates pipelines and uploads meshes.
         * @param inLevelDescription Description of the level to load.
         */
        void load(projectv::core::IRenderer& inOutRenderer, const projectv::core::LevelDesc& inLevelDescription);

    private:
        /// Loads the two shaders and creates the named pipeline; reuses it if the name is already cached.
        void readPipeline(projectv::core::IRenderer& inOutRenderer, const projectv::core::PipelineDesc& inPipelineDesc);

        /// Spawns one entity, choosing the handler from the type held by EntityDesc::data.
        void readData(const projectv::core::EntityDesc& entity, projectv::core::IRenderer& inOutRenderer);

        void spawnStaticMesh(const projectv::core::EntityDesc& entityDesc, const projectv::core::StaticMeshDesc& smDesc, projectv::core::IRenderer& inOutRenderer);

        void spawnSkybox(const projectv::core::EntityDesc& entityDesc, const projectv::core::SkyboxDesc& skyboxDesc, projectv::core::IRenderer& inOutRenderer);

        World& world;

        MeshManager& meshManager;

        ShaderManager& shaderManager;

        /// Pipelines created so far, by PipelineDesc::name.
        std::unordered_map<std::string, projectv::core::GraphicsPipelineHandle> graphicsPipelineCache;

        /// GPU meshes uploaded so far, by CPU mesh handle index.
        std::unordered_map<std::uint32_t, projectv::core::gpu::GpuMeshHandle> cpuGpuMeshMap;
    };
}
