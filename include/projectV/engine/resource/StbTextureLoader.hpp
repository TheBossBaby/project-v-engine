#pragma once

#include <optional>

#include <projectV/core/textureLoader.hpp>

namespace projectv::engine
{
    /**
     * @brief Texture loader implementation based on stb_image.
     *
     * Decodes the image formats supported by stb_image (PNG, JPEG, TGA,
     * BMP, PSD, GIF, PIC, PNM) into the engine's Texture format. Every
     * image is expanded to four channels and stored as
     * TextureFormat::R8G8B8A8_UNorm, whatever its channel count on disk.
     *
     * @note HDR images (.hdr) are not supported yet. Loading one fails
     *       instead of silently tone-mapping it down to 8 bits per channel.
     *
     * @note This loader does not generate mipmaps, so
     *       TextureLoadParams::bGenerateMipmaps is ignored. The mip chain
     *       has to be built when the texture is uploaded to the GPU.
     */
    class StbTextureLoader : public projectv::core::ITextureLoader
    {
    public:
        /**
         * @brief Load a texture using stb_image.
         *
         * Decodes the image at params.path, optionally flipping it
         * vertically, and copies the RGBA8 pixels into Texture::data.
         * Texture::id is left at its default value; it is not assigned
         * by the loader.
         *
         * @param params
         *        Texture loading parameters, including the source path and
         *        whether to flip the image vertically.
         *
         * @return The loaded Texture on success; otherwise std::nullopt.
         */
        std::optional<projectv::core::Texture> loadTexture(const projectv::core::TextureLoadParams& params) override;
    };
}
