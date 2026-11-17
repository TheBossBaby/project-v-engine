#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <projectV/core/texture.hpp>
#include <projectV/core/textureHandle.hpp>
#include <projectV/core/textureLoader.hpp>

namespace projectv::engine
{
    /**
     * @brief Manages loading and caching Texture resources.
     *
     * TextureManager owns all loaded Texture instances and returns lightweight
     * TextureHandle values for lookup via get(). Textures are cached by file
     * path: the first successful load for a given path is stored, and later
     * calls with the same path return the same handle.
     *
     * The manager is API-agnostic: it holds CPU-side pixel data only and does
     * not know about GPU objects or rendering backends.
     *
     * Note:
     * - The cache key is currently the file path only. TextureLoadParams flags
     *   such as bFlipVertical and bGenerateMipmaps are passed to the loader on
     *   the *first* load for a path, but they are ignored for later calls with
     *   the same path. This means you cannot have several variants of the same
     *   file (e.g. flipped and non-flipped) loaded at the same time via
     *   TextureManager in the current version.
     */
    class TextureManager
    {
    public:
        /**
         * @brief Construct a new TextureManager.
         *
         * Takes ownership of the given ITextureLoader instance, which is used
         * internally to load textures from disk on demand.
         */
        explicit TextureManager(std::unique_ptr<projectv::core::ITextureLoader> loader);

        /**
         * @brief Load a texture (if needed) and return its handle.
         *
         * If a texture for params.path has not been loaded yet, this function
         * calls the underlying ITextureLoader with the provided
         * TextureLoadParams, stores the resulting Texture in an internal
         * array, and returns a TextureHandle pointing to it.
         *
         * If a texture for params.path has already been loaded, this function
         * returns the existing TextureHandle from the cache and does *not*
         * reload or modify the stored texture.
         *
         * Note:
         * - The cache key is params.path only. If you call load() several
         *   times with the same path but different TextureLoadParams flags,
         *   the first successful load "wins" and later calls reuse that same
         *   texture.
         *
         * @param params Texture loading parameters, including the file path
         *               and loader flags (e.g. bFlipVertical, bGenerateMipmaps).
         * @return A valid TextureHandle on success, or an invalid handle if loading fails.
         */
        projectv::core::TextureHandle load(const core::TextureLoadParams& params);

        /**
         * @brief Look up a previously loaded texture by handle.
         *
         * Validates the given TextureHandle and returns a pointer to the
         * corresponding Texture stored internally.
         *
         * @param handle TextureHandle previously returned by load().
         * @return Pointer to the Texture if the handle is valid and in range,
         *         or nullptr if the handle is invalid or out of bounds.
         */
        const core::Texture* get(core::TextureHandle handle) const;

    private:
        /// Underlying loader used to import textures from disk.
        std::unique_ptr<core::ITextureLoader> textureLoader;

        /// Contiguous storage for all loaded textures.
        std::vector<core::Texture> textureList;

        /// Cache mapping file path -> TextureHandle (path-only key, see note above).
        std::unordered_map<std::string, core::TextureHandle> cache;
    };
}
