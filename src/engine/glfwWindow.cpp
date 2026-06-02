#include "glfwWindow.hpp"

projectv::engine::GlfwWindow::GlfwWindow()
{
}

projectv::engine::GlfwWindow::~GlfwWindow()
{
}

bool projectv::engine::GlfwWindow::init(const projectv::core::WindowDesc &desc)
{
    return false;
}

void projectv::engine::GlfwWindow::shutdown()
{
}

void projectv::engine::GlfwWindow::resize(uint32_t width, uint32_t height)
{
}

void projectv::engine::GlfwWindow::pollEvents()
{
}

bool projectv::engine::GlfwWindow::shouldClose() const
{
    return false;
}

uint32_t projectv::engine::GlfwWindow::getWidth() const
{
    return 0;
}

uint32_t projectv::engine::GlfwWindow::getHeight() const
{
    return 0;
}
