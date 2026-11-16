#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <optional>
#include <string>

// The stb_image implementation is compiled into this file only.
// STB_IMAGE_STATIC keeps its functions local to this file, so they cannot
// clash with the copy of stb_image that Assimp compiles into its library.
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <projectV/core/texture.hpp>

#include <projectV/engine/resource/StbTextureLoader.hpp>
#include <projectV/engine/logging/log.hpp>

namespace projectv::engine
{
    namespace
    {
        /// Every image is expanded to RGBA so it matches TextureFormat::R8G8B8A8_UNorm.
        constexpr int RequestedChannels = STBI_rgb_alpha;

        /// Frees pixel buffers allocated by stb_image.
        struct StbiImageDeleter
        {
            void operator()(stbi_uc* pixels) const { stbi_image_free(pixels); }
        };

        using StbiImagePtr = std::unique_ptr<stbi_uc, StbiImageDeleter>;

        /// Returns stb_image's reason for the last failure on this thread.
        const char* lastFailureReason()
        {
            const char* reason = stbi_failure_reason();
            return reason ? reason : "unknown error";
        }
    }

    std::optional<projectv::core::Texture> StbTextureLoader::loadTexture(const projectv::core::TextureLoadParams& params)
    {
        if (params.path.empty())
        {
            LogError("StbTextureLoader: texture path is empty.");
            return std::nullopt;
        }

        const std::string& path = params.path;

        if (stbi_is_hdr(path.c_str()))
        {
            LogError(std::format("StbTextureLoader: '{}' is an HDR image, which is not supported yet.", path));
            return std::nullopt;
        }

        // The per-thread setting keeps this load from changing the flip
        // state of loads running on other threads.
        stbi_set_flip_vertically_on_load_thread(params.bFlipVertical ? 1 : 0);

        int width = 0;
        int height = 0;
        int channelsInFile = 0;

        StbiImagePtr pixels(stbi_load(path.c_str(), &width, &height, &channelsInFile, RequestedChannels));

        if (!pixels)
        {
            LogError(std::format("StbTextureLoader: failed to load '{}': {}.", path, lastFailureReason()));
            return std::nullopt;
        }

        projectv::core::Texture texture;
        texture.desc.width  = static_cast<std::uint32_t>(width);
        texture.desc.height = static_cast<std::uint32_t>(height);
        texture.desc.format = projectv::core::TextureFormat::R8G8B8A8_UNorm;
        texture.desc.usage  = projectv::core::TextureUsage::Sampled;

        const std::size_t byteCount =
            static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * RequestedChannels;

        texture.data.assign(pixels.get(), pixels.get() + byteCount);

        LogInfo(std::format("StbTextureLoader: loaded '{}' ({}x{}, {} channel(s) in file).",
            path, width, height, channelsInFile));

        return texture;
    }
}
