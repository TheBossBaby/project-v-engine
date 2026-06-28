#pragma once

#include <projectV/core/shaderLoader.hpp>

namespace projectv::core
{
    struct Shader;
}

namespace projectv::engine
{
    /**
     * @brief Loads shader data from the filesystem into a core shader asset.
     *
     * This loader reads a shader file from disk using the path provided in
     * ShaderLoadParams, performs basic validation (path, stage, file existence,
     * non-empty content), and returns a populated Shader object on success.
     * The raw file contents are stored in Shader::data as a byte buffer, without
     * interpreting the format (for example, GLSL text or SPIR-V binary).
     *
     * The resulting Shader can be consumed by rendering backends, which are
     * responsible for interpreting the byte data and creating API-specific
     * GPU shader objects.
     */
    class FileShaderLoader : public projectv::core::IShaderLoader
    {
    public:
        /**
         * @brief Load a shader from a file on disk.
         *
         * This function opens the file at the given path, reads its contents
         * into a byte buffer, and fills a Shader structure with the supplied
         * stage, debug name, and path. If the path is empty, the shader stage
         * is invalid, the file does not exist, is not a regular file, or is
         * empty, the function logs an error and returns std::nullopt.
         *
         * @param params Shader loading parameters, including file path,
         *        shader stage, and optional debug name.
         * @return A populated Shader on success, or std::nullopt if loading
         *         fails for any reason.
         */
        std::optional<projectv::core::Shader>
        loadShader(const projectv::core::ShaderLoadParams& params) override;
    };
}