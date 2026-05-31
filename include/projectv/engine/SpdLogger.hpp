
#pragma once
#include "projectV/core/logger.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace spdlog { class logger; }

namespace projectv::engine
{
    class SpdLogger : public projectv::core::ILogger
    {
    public:
        /// @brief Construct a logger that forwards calls to the given spdlog logger.
        /// @param logger Shared pointer to a configured spdlog logger instance.
        explicit SpdLogger(std::shared_ptr<spdlog::logger> logger);

        void log(projectv::core::LogLevel level, std::string_view message) override;

        /// @copydoc projectv::core::ILogger::trace 
        void trace(std::string_view message)  override;

        /// @copydoc projectv::core::ILogger::info 
        void info(std::string_view message)   override;

        /// @copydoc projectv::core::ILogger::warn 
        void warn(std::string_view message)   override;

        /// @copydoc projectv::core::ILogger::error 
        void error(std::string_view message)  override;

        /// @copydoc projectv::core::ILogger::critical 
        void critical(std::string_view message) override;
    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };

    /// @brief Create a spdlog-backed ILogger instance.
    /// @param loggerName Name used to identify the underlying spdlog logger.
    std::unique_ptr<projectv::core::ILogger> CreateSpdLogger(const std::string& loggerName = "SpdLogger");
}