#include "projectV/engine/log.hpp"

int main()
{
    projectv::engine::InitLogging();
    projectv::engine::LogInfo("Application started.");
    projectv::engine::ShutdownLogging();

    return 0;
}