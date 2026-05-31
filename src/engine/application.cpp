#include "projectv/engine/application.hpp"
#include "projectv/engine/log.hpp"

namespace projectv::engine
{
    bool Application::init(const projectv::core::WindowDesc& inWindowDesc)
    {
        LogInfo("Application::init called");
        return true;
    }

    void Application::run()
    {
        LogInfo("Application::run starting main loop");
        render();
    }

    void Application::shutdown()
    {
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