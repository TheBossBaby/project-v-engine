#include <cstdint>
#include <format>
#include <string>
#include <utility>

#include <projectV/engine/logging/log.hpp>

#include <projectV/core/textureHandle.hpp>
#include <projectV/engine/resource/textureManager.hpp>

namespace projectv::engine
{
    TextureManager::TextureManager(std::unique_ptr<projectv::core::ITextureLoader> loader)
    : textureLoader(std::move(loader))
    {
    }

    projectv::core::TextureHandle TextureManager::load(const core::TextureLoadParams& params)
    {
        auto it = cache.find(params.path);
        if(it != cache.end()) return it->second;

        if(!textureLoader)
        {
            LogError("TextureManager: loader is null, cannot load texture.");
            return projectv::core::TextureHandle{};
        }

        auto loadedTexture = textureLoader->loadTexture(params);

        if(!loadedTexture)
        {
            LogError(std::format("TextureManager: failed to load texture '{}'.", params.path));
            return projectv::core::TextureHandle{};
        }

        projectv::core::TextureHandle handle;
        handle.index = static_cast<std::uint32_t>(textureList.size());

        textureList.push_back(std::move(*loadedTexture));
        cache.emplace(params.path, handle);

        return handle;
    }

    const core::Texture* TextureManager::get(core::TextureHandle handle) const
    {
        if (!handle.isValid())
        {
            LogError("TextureManager: invalid texture handle.");
            return nullptr;
        }

        if (handle.index >= textureList.size())
        {
            LogError("TextureManager: handle is out of bound");
            return nullptr;
        }

        return &textureList[handle.index];
    }
}
