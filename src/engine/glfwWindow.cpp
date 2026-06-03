#include "projectV/engine/platform/glfwWindow.hpp"
#include <GLFW/glfw3.h>

namespace projectv::engine
{
    GlfwWindow::GlfwWindow()
    {
    }

    GlfwWindow::~GlfwWindow()
    {
    }

    bool GlfwWindow::init(const projectv::core::WindowDesc &desc)
    {
        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Vulkan, not OpenGL
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //Todo : Make it resizable, pass it from CMake 

        m_width = desc.width;
        m_height = desc.height;

        m_window = glfwCreateWindow(static_cast<int>(desc.width),
                                static_cast<int>(desc.height),
                                desc.windowName.data(), nullptr, nullptr);

        if (!m_window)
        {
            glfwTerminate();
            return false;
        }

        return true;
    }

    void GlfwWindow::shutdown()
    {
        if(!m_window) return;

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void GlfwWindow::resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        // TODO: integrate with GLFW framebuffer size callback if needed
    }

    void GlfwWindow::pollEvents()
    {
        glfwPollEvents();
    }

    bool GlfwWindow::shouldClose() const
    {
        if(!m_window) return true;
        return glfwWindowShouldClose(m_window) == GLFW_TRUE;
    }

    uint32_t GlfwWindow::getWidth() const
    {
        if(!m_window) return 0;
        return m_width;
    }

    uint32_t GlfwWindow::getHeight() const
    {
        if(!m_window) return 0;
        return m_height;
    }
}