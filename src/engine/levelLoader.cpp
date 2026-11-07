#include <projectV/core/graphicsPipelineDescription.hpp>
#include <projectV/core/renderer.hpp>
#include <projectV/core/transform.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>
#include <projectV/engine/resource/shaderManager.hpp>
#include <projectV/engine/resource/meshManager.hpp>

#include <projectV/engine/resource/levelLoader.hpp>

namespace projectv::engine
{
    LevelLoader::LevelLoader(World &inWorld, MeshManager &inMeshManager, ShaderManager &inShaderManager)
    : world(inWorld), meshManager(inMeshManager), shaderManager(inShaderManager)
    {

    }

    void LevelLoader::load(projectv::core::IRenderer& inOutRenderer)
    {
        for(int i = 0; i <= 1; i++)
        {
            projectv::core::ShaderHandle vertexShaderHandle = shaderManager.load(
                projectv::core::ShaderLoadParams{"D:/P/project-v-game/assets/shader/mesh.vert",
                    projectv::core::ShaderStage::Vertex,
                    "MeshVertex",
                    true});

            if(!vertexShaderHandle.isValid())
            {
                LogError("LevelLoader::load, VertexShader handle is invalid skipping this.");
                continue;
            }

            projectv::core::ShaderHandle fragmentShaderHandle = shaderManager.load(
                projectv::core::ShaderLoadParams{"D:/P/project-v-game/assets/shader/triangle.frag",
                    projectv::core::ShaderStage::Fragment,
                    "TriangleFragment",
                    true});                
            
            if(!fragmentShaderHandle.isValid())
            {
                LogError("LevelLoader::load, FragmentShader handle is invalid skipping this");
                continue;
            }

            projectv::core::GraphicsPipelineHandle graphicsPipeline;
            projectv::core::gpu::GpuMeshHandle     gpuMeshHandle;

            const std::uint64_t key = (static_cast<std::uint64_t>(vertexShaderHandle.index) << 32) | fragmentShaderHandle.index;
            auto gpEntry = graphicsPipelineCache.find(key);

            if (gpEntry == graphicsPipelineCache.end()) 
            {
                LogInfo("LevelLoader::load, combination of shaders is new, creating GraphicsPipeline");
                projectv::core::GraphicsPipelineDescription graphicsPipelineDescription = 
                {
                    shaderManager.get(vertexShaderHandle)->data, 
                    shaderManager.get(fragmentShaderHandle)->data
                };

                graphicsPipeline = inOutRenderer.createGraphicsPipeline(graphicsPipelineDescription);
                if(!graphicsPipeline.isValid())
                {
                    LogError("LevelLoader::load, combination of shaders, GraphicsPipelineCreation failed hence skipping this combination");
                    continue;  
                }
                graphicsPipelineCache.insert({key, graphicsPipeline});
            }
            else
            {
                LogTrace("LevelLoader::load, combination of shader is present, reusing GraphicsPipeline");
                graphicsPipeline = gpEntry->second;
            }

            projectv::core::MeshHandle meshHandle = meshManager.load(\
                projectv::core::MeshLoadParams{
                    "D:/P/project-v-game/assets/mesh/cube.fbx"});

            if(!meshHandle.isValid())
            {
                LogError("LevelLoader::load, Mesh handle is invalid skipping this");
                continue;
            }

            auto gpuMeshEntry = cpuGpuMeshMap.find(meshHandle.index);

            if (gpuMeshEntry == cpuGpuMeshMap.end()) 
            {
                LogInfo("LevelLoader::load, CPU Mesh is not loaded in GPU, Loading now");
                gpuMeshHandle = inOutRenderer.uploadMesh(*meshManager.get(meshHandle));

                if(!gpuMeshHandle.isValid())
                {
                    LogError("LevelLoader::load, Uploading of Mesh Failed hence skipping this MeshEntry");
                    continue;  
                }

                cpuGpuMeshMap.insert( {meshHandle.index, gpuMeshHandle} );
            }
            else
            {
                LogTrace("LevelLoader::load, CPU Mesh is present in GPU, reusing GpuMesh Handle");
                gpuMeshHandle = gpuMeshEntry->second;
            }       

            projectv::core::Transform transform;
            transform.position = {i*1, i*2, i*3};

            auto entity = world.createEntity();
            world.addComponent<projectv::core::gpu::GpuMeshHandle>(entity, gpuMeshHandle);
            world.addComponent<projectv::core::Transform>(entity, transform);
            world.addComponent<projectv::core::GraphicsPipelineHandle>(entity, graphicsPipeline);
        }
    }
}
