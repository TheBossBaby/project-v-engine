#include "projectV/core/logger.hpp"

#include "projectV/engine/logging/log.hpp"
#include "projectV/engine/logging/SpdLogger.hpp"

#include <memory>


namespace projectv::engine
{
    namespace
    {
        std::unique_ptr<projectv::core::ILogger> g_logger;
    }

    void InitLogging()
    {
        g_logger = CreateSpdLogger();
    }

    void ShutdownLogging()
    {
        g_logger.reset();
    }

    void LogInfo(std::string_view message)
    {
        if (g_logger) g_logger->info(message);
    }

    void LogTrace(std::string_view message)
    {
        if (g_logger) g_logger->trace(message);
    }

    void LogWarn(std::string_view message)
    {
        if (g_logger) g_logger->warn(message);
    }

    void LogError(std::string_view message)
    {
        if (g_logger) g_logger->error(message);
    }

    void LogCritical(std::string_view message)
    {
        if (g_logger) g_logger->critical(message);
    }
}