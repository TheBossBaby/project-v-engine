#include <projectV/engine/logging/log.hpp>

#include <projectV/core/meshHandle.hpp>
#include <projectV/engine/resource/meshManager.hpp>

namespace projectv::engine
{
    MeshManager::MeshManager(std::unique_ptr<projectv::core::IMeshLoader> loader)
    : meshLoader(std::move(loader))
    {
    }

    projectv::core::MeshHandle MeshManager::load(const core::MeshLoadParams &params)
    {
        auto it = cache.find(std::string(params.path.data()));
        if(it != cache.end()) return it->second;

        if(!meshLoader)
        {
            LogError("MeshManager: loader is null, cannot load mesh.");
            return projectv::core::MeshHandle{};
        }

        auto loadedMesh = meshLoader->loadMesh(params);

        if(!loadedMesh)
        {
            LogError("MeshManager: failed to load mesh.");
            return projectv::core::MeshHandle{};
        }

        projectv::core::MeshHandle handle;
        handle.index = static_cast<uint32_t>(meshList.size());

        meshList.push_back(std::move(*loadedMesh));
        cache.emplace(std::string(params.path.data()), handle);

        return handle;
    }

    const core::Mesh* MeshManager::get(core::MeshHandle handle) const
    {
        if (!handle.isValid())
        {
            LogError("MeshManager: invalid mesh handle.");
            return nullptr;
        }

        if (handle.index >= meshList.size())
        {
            LogError("MeshManager: handle is out of bound");
            return nullptr;
        }

        return &meshList[handle.index];
    }
}