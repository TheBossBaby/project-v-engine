#include "projectv/engine/log.hpp"

int main()
{
    projectv::engine::InitLogging();
    projectv::engine::LogInfo("Editor starting...");
    projectv::engine::ShutdownLogging();
    return 0;
}