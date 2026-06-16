#include <projectV/core/shader.hpp>

#include <projectV/engine/resource/fileShaderLoader.hpp>
#include <projectV/engine/logging/log.hpp>

#include <filesystem>
#include <fstream>
#include <format>
#include <iterator>
#include <optional>
#include <vector>

namespace projectv::engine
{
    std::optional<projectv::core::Shader> FileShaderLoader::loadShader(const projectv::core::ShaderLoadParams &params)
    {
        using namespace projectv;

        if (params.path.empty())
        {
            engine::LogError(std::format("Shader path is empty."));
            return std::nullopt;
        }

        if (params.stage == core::ShaderStage::None)
        {
            engine::LogError(std::format("Shader '{}' has invalid shader stage.", params.path));
            return std::nullopt;
        }

        std::filesystem::path shaderPath(params.path);

        if (!std::filesystem::exists(shaderPath))
        {
            engine::LogError(std::format("Shader file '{}' does not exist.",shaderPath.string()));
            return std::nullopt;
        }

        if (!std::filesystem::is_regular_file(shaderPath))
        {
            engine::LogError(std::format("'{}' is not a regular file.",shaderPath.string()));
            return std::nullopt;
        }

        std::ifstream file(shaderPath, std::ios::binary | std::ios::ate);

        if (!file.is_open())
        {
            engine::LogError(std::format("Failed to open shader '{}'.",shaderPath.string()));
            return std::nullopt;
        }

        const std::streamsize fileSize = file.tellg();

        if (fileSize <= 0)
        {
            engine::LogError(std::format("Shader '{}' is empty.",shaderPath.string()));
            return std::nullopt;
        }

        std::vector<std::uint8_t> shaderData(static_cast<size_t>(fileSize));

        file.seekg(0, std::ios::beg);

        if (!file.read(reinterpret_cast<char*>(shaderData.data()), fileSize))
        {
            engine::LogError(std::format("Failed to read shader '{}'.",shaderPath.string()));
            return std::nullopt;
        }

        file.close();

        if (params.debugName.empty())
        {
            engine::LogWarn(std::format("Shader '{}' loaded without a debug name.",shaderPath.string()));
        }

        core::Shader shader;
        shader.path = params.path;
        shader.debugName = params.debugName;
        shader.stage = params.stage;
        shader.data = std::move(shaderData);

        engine::LogInfo(std::format("Loaded shader '{}' ({} bytes).",shaderPath.string(),shader.data.size()));

        return shader;
    }
}