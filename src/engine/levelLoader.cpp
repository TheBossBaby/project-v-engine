#include <exception>
#include <filesystem>
#include <string>
#include <variant>

#include <projectV/core/graphicsPipelineDescription.hpp>
#include <projectV/core/renderer.hpp>
#include <projectV/core/transform.hpp>
#include <projectV/core/level/levelDesc.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>
#include <projectV/engine/resource/shaderManager.hpp>
#include <projectV/engine/resource/meshManager.hpp>

#include <projectV/engine/resource/levelLoader.hpp>
#include <projectV/engine/util/assetPath.hpp>
#include <projectV/engine/util/assets.hpp>

namespace projectv::engine
{
    namespace
    {
        template<class... Ts>
        struct Overloaded : Ts... { using Ts::operator()...; };

        template<class... Ts>
        Overloaded(Ts...) -> Overloaded<Ts...>;

        /// Resolves an asset path from a description against ASSETROOT; throws std::invalid_argument if it is not allowed.
        std::string resolveAsset(const std::string& inRelativePath)
        {
            return ResolveAssetPath(std::filesystem::path(ASSETROOT), std::filesystem::path(inRelativePath)).generic_string();
        }
    }

    LevelLoader::LevelLoader(World &inWorld, MeshManager &inMeshManager, ShaderManager &inShaderManager)
    : world(inWorld), meshManager(inMeshManager), shaderManager(inShaderManager)
    {

    }

    void LevelLoader::load(projectv::core::IRenderer& inOutRenderer, const projectv::core::LevelDesc& inLevelDescription)
    {
        for(const auto& pipelineDesc : inLevelDescription.pipelineList)
        {
            try
            {
                readPipeline(inOutRenderer, pipelineDesc);
            }
            catch(const std::exception& e)
            {
                LogError("LevelLoader::load, pipeline '" + pipelineDesc.name + "' failed, skipping it: " + e.what());
            }
        }

        for(const auto& entityDesc : inLevelDescription.entityList)
        {
            try
            {
                readData(entityDesc, inOutRenderer);
            }
            catch(const std::exception& e)
            {
                LogError("LevelLoader::load, entity '" + entityDesc.name + "' failed, skipping it: " + e.what());
            }
        }
    }

    void LevelLoader::readPipeline(projectv::core::IRenderer& inOutRenderer, const projectv::core::PipelineDesc& inPipelineDesc)
    {
        if(graphicsPipelineCache.contains(inPipelineDesc.name))
        {
            LogWarn("LevelLoader::readPipeline, pipeline '" + inPipelineDesc.name + "' is already created, reusing it");
            return;
        }

        projectv::core::ShaderHandle vertexShaderHandle = shaderManager.load(
            projectv::core::ShaderLoadParams{
                resolveAsset(inPipelineDesc.vertexShaderPath),
                projectv::core::ShaderStage::Vertex,
                inPipelineDesc.name + "_vert",
                true});

        if(!vertexShaderHandle.isValid())
        {
            LogError("LevelLoader::readPipeline, vertex shader of pipeline '" + inPipelineDesc.name + "' is invalid, skipping it");
            return;
        }

        projectv::core::ShaderHandle fragmentShaderHandle = shaderManager.load(
            projectv::core::ShaderLoadParams{
                resolveAsset(inPipelineDesc.fragmentShaderPath),
                projectv::core::ShaderStage::Fragment,
                inPipelineDesc.name + "_frag",
                true});

        if(!fragmentShaderHandle.isValid())
        {
            LogError("LevelLoader::readPipeline, fragment shader of pipeline '" + inPipelineDesc.name + "' is invalid, skipping it");
            return;
        }

        LogInfo("LevelLoader::readPipeline, creating GraphicsPipeline '" + inPipelineDesc.name + "'");
        projectv::core::GraphicsPipelineDescription graphicsPipelineDescription =
        {
            shaderManager.get(vertexShaderHandle)->data,
            shaderManager.get(fragmentShaderHandle)->data
        };

        projectv::core::GraphicsPipelineHandle graphicsPipeline = inOutRenderer.createGraphicsPipeline(graphicsPipelineDescription);
        if(!graphicsPipeline.isValid())
        {
            LogError("LevelLoader::readPipeline, creating GraphicsPipeline '" + inPipelineDesc.name + "' failed, skipping it");
            return;
        }

        graphicsPipelineCache.emplace(inPipelineDesc.name, graphicsPipeline);
    }

    void LevelLoader::readData(const projectv::core::EntityDesc &entity, projectv::core::IRenderer &inOutRenderer)
    {
        std::visit(Overloaded{
            [&](const projectv::core::StaticMeshDesc& d) { spawnStaticMesh(entity, d, inOutRenderer); },
            [&](const projectv::core::SkyboxDesc&     d) { spawnSkybox(entity, d, inOutRenderer); }
        }, entity.data);
    }

    void LevelLoader::spawnStaticMesh(const projectv::core::EntityDesc &entityDesc, const projectv::core::StaticMeshDesc &smDesc, projectv::core::IRenderer& inOutRenderer)
    {
        // Check the pipeline first, so no mesh is loaded or uploaded for an entity that cannot be drawn.
        auto pipelineEntry = graphicsPipelineCache.find(smDesc.pipelineName);
        if(pipelineEntry == graphicsPipelineCache.end())
        {
            LogError("LevelLoader::spawnStaticMesh, entity '" + entityDesc.name + "' uses unknown pipeline '" + smDesc.pipelineName + "', skipping it");
            return;
        }

        projectv::core::MeshHandle meshHandle = meshManager.load(
            projectv::core::MeshLoadParams
            {
                resolveAsset(smDesc.meshPath)
            }
        );

        if(!meshHandle.isValid())
        {
            LogError("LevelLoader::spawnStaticMesh, mesh of entity '" + entityDesc.name + "' is invalid, skipping it");
            return;
        }

        projectv::core::gpu::GpuMeshHandle gpuMeshHandle;

        auto gpuMeshEntry = cpuGpuMeshMap.find(meshHandle.index);
        if(gpuMeshEntry == cpuGpuMeshMap.end())
        {
            LogInfo("LevelLoader::spawnStaticMesh, CPU mesh of entity '" + entityDesc.name + "' is not on the GPU yet, uploading it");
            gpuMeshHandle = inOutRenderer.uploadMesh(*meshManager.get(meshHandle));

            if(!gpuMeshHandle.isValid())
            {
                LogError("LevelLoader::spawnStaticMesh, uploading the mesh of entity '" + entityDesc.name + "' failed, skipping it");
                return;
            }

            cpuGpuMeshMap.emplace(meshHandle.index, gpuMeshHandle);
        }
        else
        {
            LogTrace("LevelLoader::spawnStaticMesh, CPU mesh is already on the GPU, reusing the GpuMesh handle");
            gpuMeshHandle = gpuMeshEntry->second;
        }

        const projectv::core::Transform transform = entityDesc.transform.value_or(projectv::core::Transform{});

        auto entity = world.createEntity();
        world.addComponent<projectv::core::gpu::GpuMeshHandle>(entity, gpuMeshHandle);
        world.addComponent<projectv::core::Transform>(entity, transform);
        world.addComponent<projectv::core::GraphicsPipelineHandle>(entity, pipelineEntry->second);
    }

    void LevelLoader::spawnSkybox(const projectv::core::EntityDesc &entityDesc, const projectv::core::SkyboxDesc &, projectv::core::IRenderer &)
    {
        LogWarn("LevelLoader::spawnSkybox, skybox '" + entityDesc.name + "' skipped, not implemented yet");
    }
}
