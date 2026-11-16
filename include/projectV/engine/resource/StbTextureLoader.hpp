#pragma once

#include <optional>

#include <projectV/core/textureLoader.hpp>

namespace projectv::engine
{
    /**
     * @brief Texture loader implementation based on stb_image.
     *
     * Decodes the image formats supported by stb_image into the engine's
     * Texture format. Every image is expanded to four channels (RGBA),
     * whatever its channel count on disk:
     * - Radiance HDR images (.hdr) keep their linear floating-point values
     *   and are stored as TextureFormat::R32G32B32A32_Float. Their alpha
     *   channel is set to 1.0.
     * - All other formats (PNG, JPEG, TGA, BMP, PSD, GIF, PIC, PNM) are
     *   stored as TextureFormat::R8G8B8A8_UNorm.
     *
     * @note OpenEXR images (.exr) are not supported by stb_image, so loading
     *       one fails. They need a separate loader, such as one based on
     *       TinyEXR.
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
         * vertically, and copies the RGBA pixels into Texture::data.
         * Texture::desc.format tells whether they are RGBA8 or RGBA32 float.
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
