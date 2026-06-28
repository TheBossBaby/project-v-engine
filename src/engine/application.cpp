#include <projectV/engine/core/application.hpp>
#include <projectV/engine/platform/glfwWindow.hpp>
#include <projectV/engine/logging/log.hpp>

namespace projectv::engine
{
    bool Application::init(const projectv::core::WindowDesc& inWindowDesc)
    {
        m_windowDesc = inWindowDesc;

        m_window = std::make_unique<GlfwWindow>();

        if (!m_window->init(m_windowDesc))
        {
            LogError("Application::init - failed to initialize window");
            m_window.reset();
            return false;
        }

        LogInfo("Application::init called");
        return true;
    }

    void Application::run()
    {
        LogInfo("Application::run starting main loop");

        // Simple fixed-step loop for now
        const float fixedDeltaTime = 1.0f / 60.0f;

        while (m_window && !m_window->shouldClose())
        {
            m_window->pollEvents();

            tick(fixedDeltaTime);
            render();
        }
    }

    void Application::shutdown()
    {
        if (!m_window) return;

        m_window->shutdown();
        m_window.reset();

        LogInfo("Application::shutdown called");
    }

    void Application::tick(float deltaTime)
    {
        LogInfo("Application::tick");
    }

    void Application::render()
    {
        LogInfo("Application::render");
    }
}