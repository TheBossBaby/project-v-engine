#include "projectV/engine/log.hpp"
#include "projectV/engine/application.hpp"

int main()
{
    projectv::engine::InitLogging();
    projectv::engine::LogInfo("Application started.");

    // TODO: : Move to config
    const std::uint32_t windowWidth  = 400;
    const std::uint32_t windowHeight = 400;
    const std::string_view windowName = "Project-V-App";

    projectv::engine::Application app;
    projectv::core::WindowDesc windowDesc{windowWidth, windowHeight, windowName};

    if (!app.init(windowDesc))
    {
        projectv::engine::LogError("Failed to initialize Application");
        return 1;
    }

    app.run();
    app.shutdown();
    
    projectv::engine::ShutdownLogging();

    return 0;
}