#include <projectV/engine/logging/log.hpp>

#include <projectv/core/shaderHandle.hpp>
#include <projectV/engine/resource/shaderManager.hpp>

namespace projectv::engine
{
    ShaderManager::ShaderManager(std::unique_ptr<projectv::core::IShaderLoader> loader)
    : loader(std::move(loader))
    {
    }

    projectv::core::ShaderHandle ShaderManager::load(const projectv::core::ShaderLoadParams& params)
    {
        //TODO : Hash the path to make unique Key
        auto it = cache.find(params.path);
        if(it != cache.end()) return it->second;

        if(!loader)
        {
            LogError("ShaderManager: loader is null, cannot load shader.");
            return projectv::core::ShaderHandle{};
        }

        auto loadedShader = loader->loadShader(params);

        if(!loadedShader)
        {
            LogError("ShaderManager: failed to load shader.");
            return projectv::core::ShaderHandle{};
        }

        projectv::core::ShaderHandle handle;
        handle.index = static_cast<uint32_t>(shaderList.size());

        shaderList.push_back(std::move(*loadedShader));
        cache.emplace(params.path, handle);

        return handle;
    }

    const core::Shader* ShaderManager::get(core::ShaderHandle handle) const
    {
        if (!handle.isValid())
        {
            LogError("ShaderManager: invalid shader handle.");
            return nullptr;
        }

        if (handle.index >= shaderList.size())
        {
            LogError("ShaderManager: handle is out of bound");
            return nullptr;
        }

        return &shaderList[handle.index];
    }
}