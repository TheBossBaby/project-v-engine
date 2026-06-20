#pragma once
#include <string_view>

namespace projectv::engine
{
    /// @brief Initialize the engine logging system using the default backend.
    /// @note Must be called once before any LogInfo/LogWarn/LogError calls.
    void InitLogging();
    
    /// @brief Shutdown the engine logging system and release logging resources.
    void ShutdownLogging();

    /// @brief Log an informational message through the engine logger.
    /// @param message Text to log at info level. 
    void LogInfo(std::string_view message);

    /// @brief Log a tracing message through the engine logger.
    /// @param message Text to log at trace level.
    void LogTrace(std::string_view message);

    /// @brief Log a warning message through the engine logger.
    /// @param message Text to log at warn level.
    void LogWarn(std::string_view message);

    /// @brief Log a error message through the engine logger.
    /// @param message Text to log at error level.
    void LogError(std::string_view message);

    /// @brief Log a critical message through the engine logger.
    /// @param message Text to log at critical level.
    void LogCritical(std::string_view message);
}