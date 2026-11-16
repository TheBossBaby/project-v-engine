#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

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
        /// Every image is expanded to four channels (RGBA).
        constexpr int RequestedChannels = STBI_rgb_alpha;

        /// Frees pixel buffers allocated by stb_image (8-bit or float).
        struct StbiImageDeleter
        {
            void operator()(void* pixels) const { stbi_image_free(pixels); }
        };

        template <typename Channel>
        using StbiImagePtr = std::unique_ptr<Channel, StbiImageDeleter>;

        /// Returns stb_image's reason for the last failure on this thread.
        const char* lastFailureReason()
        {
            const char* reason = stbi_failure_reason();
            return reason ? reason : "unknown error";
        }

        /**
         * @brief Decode an image with stb_image and build a Texture from it.
         *
         * @tparam Channel Type of one channel in the decoded image:
         *                 stbi_uc for stbi_load, float for stbi_loadf.
         *
         * @param load   stb_image load function that returns Channel pixels.
         * @param path   Path of the image to decode.
         * @param format Texture format matching four channels of Channel.
         *
         * @return The decoded Texture on success; otherwise std::nullopt.
         */
        template <typename Channel, typename LoadFunction>
        std::optional<projectv::core::Texture> decodeImage(LoadFunction load,
                                                           const std::string& path,
                                                           projectv::core::TextureFormat format)
        {
            int width = 0;
            int height = 0;
            int channelsInFile = 0;

            StbiImagePtr<Channel> pixels(load(path.c_str(), &width, &height, &channelsInFile, RequestedChannels));

            if (!pixels)
            {
                LogError(std::format("StbTextureLoader: failed to load '{}': {}.", path, lastFailureReason()));
                return std::nullopt;
            }

            projectv::core::Texture texture;
            texture.desc.width  = static_cast<std::uint32_t>(width);
            texture.desc.height = static_cast<std::uint32_t>(height);
            texture.desc.format = format;
            texture.desc.usage  = projectv::core::TextureUsage::Sampled;

            const std::size_t byteCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height)
                                        * RequestedChannels * sizeof(Channel);

            texture.data.resize(byteCount);
            std::memcpy(texture.data.data(), pixels.get(), byteCount);

            LogInfo(std::format("StbTextureLoader: loaded '{}' ({}x{}, {} channel(s) in file, stored as {}).",
                path, width, height, channelsInFile,
                std::is_floating_point_v<Channel> ? "RGBA32 float" : "RGBA8"));

            return texture;
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

        // The per-thread setting keeps this load from changing the flip
        // state of loads running on other threads.
        stbi_set_flip_vertically_on_load_thread(params.bFlipVertical ? 1 : 0);

        // HDR images are decoded as linear floats so they keep their full
        // range; stbi_load would tone-map them down to 8 bits per channel.
        if (stbi_is_hdr(path.c_str()))
        {
            return decodeImage<float>(stbi_loadf, path, projectv::core::TextureFormat::R32G32B32A32_Float);
        }

        return decodeImage<stbi_uc>(stbi_load, path, projectv::core::TextureFormat::R8G8B8A8_UNorm);
    }
}
