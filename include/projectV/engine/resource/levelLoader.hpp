#pragma once

#include <cstdint>
#include <unordered_map>

#include <projectV/core/graphicsPipelineHandle.hpp>
#include <projectV/core/gpu/gpuMeshHandle.hpp>

namespace projectv::core
{
    class IRenderer;
}

namespace projectv::engine
{
    class ShaderManager;
    class MeshManager;
    class World;

    class LevelLoader
    {
    public:
        LevelLoader(World &inWorld, MeshManager &inMeshManager, ShaderManager &inShaderManager);

        void load(projectv::core::IRenderer& inOutRenderer);
        
        private:
        World& world;

        MeshManager& meshManager;

        ShaderManager& shaderManager;

        std::unordered_map<std::uint64_t, projectv::core::GraphicsPipelineHandle> graphicsPipelineCache;

        std::unordered_map<std::uint32_t, projectv::core::gpu::GpuMeshHandle> cpuGpuMeshMap;
    };
}
