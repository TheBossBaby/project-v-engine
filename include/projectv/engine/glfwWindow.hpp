#pragma once

#include "projectV/core/window.hpp"

struct GLFWwindow;
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
    private:
        GLFWwindow* m_window = nullptr;

        uint32_t m_width = 0;
        
        uint32_t m_height = 0;
    };
}