#pragma once


#include <projectV/core/shaderLoader.hpp>
#include <projectv/core/shaderHandle.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace projectv::engine
{
    /**
     * @brief Manages loading and caching abstract shader assets.
     *
     * The manager owns Shader instances (raw data + metadata)
     * and returns lightweight ShaderHandle values. It is API-agnostic
     * and does not know about GPU objects or rendering backends.
     */
    class ShaderManager
    {
    public:
        explicit ShaderManager(std::unique_ptr<projectv::core::IShaderLoader> loader);

        /**
         * @brief Load a shader asset (if needed) and return its handle.
         *
         * @param path     Path to the shader asset on disk.
         * @param stage    Shader stage (vertex, fragment, etc.).
         * @param format   Format of the shader data (GLSL, SPIR-V, etc.).
         * @param debugName Optional debug name for logs and tooling.
         * @return A valid ShaderHandle on success, or an invalid handle on failure.
         */
        projectv::core::ShaderHandle load(const projectv::core::ShaderLoadParams& params);

        /**
         * @brief Look up a previously loaded shader asset by handle.
         *
         * @param handle ShaderHandle returned by load().
         * @return Pointer to the Shader if valid, nullptr otherwise.
         */
        const core::Shader* get(core::ShaderHandle handle) const;
    private:
        /// Underlying loader used to import shaders from disk.
        std::unique_ptr<core::IShaderLoader> loader;

        /// Contiguous storage for all loaded meshes.
        std::vector<core::Shader> shaderList;

        /// Cache mapping file path -> MeshHandle (path-only key, see note above).
        std::unordered_map<std::string, core::ShaderHandle> cache;
    };
}