#include "projectV/engine/logging/SpdLogger.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace projectv::engine
{   
    SpdLogger::SpdLogger(std::shared_ptr<spdlog::logger> logger)
        : m_logger(std::move(logger))
    {
    }

    void SpdLogger::log(projectv::core::LogLevel level, std::string_view message)
    {
        switch (level)
        {
            case projectv::core::LogLevel::Trace:
            trace(message);
            break;

            case projectv::core::LogLevel::Info:
            info(message);
            break;

            case projectv::core::LogLevel::Warn:
            warn(message);
            break;

            case projectv::core::LogLevel::Error:
            error(message);
            break;
            
            case projectv::core::LogLevel::Critical:
            critical(message);
            break;
        }
    }

    void SpdLogger::trace(std::string_view message)
    {
        m_logger->trace(message);
    }

    void SpdLogger::info(std::string_view message)
    {
        m_logger->info(message);
    }

    void SpdLogger::warn(std::string_view message)
    {
        m_logger->warn(message);
    }

    void SpdLogger::error(std::string_view message)
    {
        m_logger->error(message);
    }

    void SpdLogger::critical(std::string_view message)
    {
        m_logger->critical(message);
    }

    std::unique_ptr<projectv::core::ILogger> CreateSpdLogger(const std::string& loggerName)
    {
        auto spd = spdlog::stdout_color_mt(loggerName);
        spdlog::set_level(spdlog::level::info);

        return std::make_unique<SpdLogger>(std::move(spd));
    }
}