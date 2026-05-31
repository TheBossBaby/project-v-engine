#include "projectV/engine/log.hpp"
#include "projectV/engine/application.hpp"

int main()
{
    projectv::engine::InitLogging();
    projectv::engine::LogInfo("Application started.");

    projectv::engine::Application app;
    projectv::core::WindowDesc windowDesc{};

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