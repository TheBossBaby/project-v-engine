#pragma once

#include "projectV/core/application.hpp"
#include <memory>

namespace projectv::engine
{
    class Application : public projectv::core::IApplication
    {
    public:
        bool init(const projectv::core::WindowDesc& windowDesc) override;

        void run() override;

        void shutdown() override;
    protected:
        void tick(float deltaTime) override;

        void render() override;
    protected:
        projectv::core::WindowDesc m_windowDesc{};
        
        std::unique_ptr<projectv::core::IWindow> m_window;
    };
}