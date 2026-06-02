#pragma once

#include "projectV/core/window.hpp"

namespace projectv::engine
{
    class GlfwWindow : public projectv::core::IWindow
    {
    public:
        GlfwWindow();

        ~GlfwWindow() override;
        
        bool init(const projectv::core::WindowDesc& desc) override;

        void shutdown() override;

        void resize(uint32_t width, uint32_t height) override;

        void pollEvents() override;

        bool shouldClose() const override;
        
        uint32_t getWidth() const override;

        uint32_t getHeight() const override;
    };
}