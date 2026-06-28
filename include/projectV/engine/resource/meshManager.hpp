#pragma once
#include <memory>

#include <projectV/core/meshLoader.hpp>
#include <projectv/core/meshHandle.hpp>
#include <unordered_map>
#include <string>

namespace projectv::engine
{
    /**
     * @brief Manages loading and caching Mesh resources.
     *
     * MeshManager owns all loaded Mesh instances and returns lightweight
     * MeshHandle values for lookup via get(). Meshes are cached by file path:
     * the first successful load for a given path is stored, and subsequent
     * calls with the same path will always return the same handle.
     *
     * Note:
     * - The cache key is currently the file path only. MeshLoadParams flags
     *   such as bFlipUVs, bCenterMesh and bNormalizeScale are passed to the
     *   loader on the *first* load for a path, but they are ignored for
     *   subsequent calls with the same path. This means you cannot have
     *   multiple variants of the same file (e.g. centered vs non‑centered)
     *   loaded at the same time via MeshManager in the current version.
     */
    class MeshManager
    {
    public:
        /**
         * @brief Construct a new MeshManager.
         *
         * Takes ownership of the given IMeshLoader instance, which is used
         * internally to load meshes from disk on demand.
         */
        explicit MeshManager(std::unique_ptr<projectv::core::IMeshLoader> loader);

        /**
         * @brief Load a mesh (if needed) and return its handle.
         *
         * If a mesh for params.path has not been loaded yet, this function
         * calls the underlying IMeshLoader with the provided MeshLoadParams,
         * stores the resulting Mesh in an internal array, and returns a
         * MeshHandle pointing to it.
         *
         * If a mesh for params.path has already been loaded, this function
         * returns the existing MeshHandle from the cache and does *not*
         * reload or modify the stored mesh.
         *
         * Note:
         * - The cache key is params.path only. If you call load() multiple
         *   times with the same path but different MeshLoadParams flags, the
         *   first successful load "wins" and later calls will reuse that
         *   same mesh.
         *
         * @param params Mesh loading parameters, including the file path and
         *               loader flags (e.g. bFlipUVs, bCenterMesh, bNormalizeScale).
         * @return A valid MeshHandle on success, or an invalid handle if loading fails.
         */
        projectv::core::MeshHandle load(const core::MeshLoadParams& params);

        /**
         * @brief Look up a previously loaded mesh by handle.
         *
         * Validates the given MeshHandle and returns a pointer to the
         * corresponding Mesh stored internally.
         *
         * @param handle MeshHandle previously returned by load().
         * @return Pointer to the Mesh if the handle is valid and in range,
         *         or nullptr if the handle is invalid or out of bounds.
         */
        const core::Mesh* get(core::MeshHandle handle) const;

    private:
        /// Underlying loader used to import meshes from disk.
        std::unique_ptr<core::IMeshLoader> meshLoader;

        /// Contiguous storage for all loaded meshes.
        std::vector<core::Mesh> meshList;

        /// Cache mapping file path -> MeshHandle (path-only key, see note above).
        std::unordered_map<std::string, core::MeshHandle> cache;
    };
}