#include "projectV/engine/resource/sceneLoader.hpp"
#include "projectV/engine/logging/log.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

namespace projectv::engine
{
    bool SceneLoader::loadFromFile(const std::string& path, World& world)
    {
        // Open the file
        std::ifstream file(path);
        if (!file.is_open())
        {
            LogError("SceneLoader: Could not open file: " + path);
            return false;
        }

        // Parse the JSON
        nlohmann::json sceneData;
        try
        {
            file >> sceneData;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            LogError("SceneLoader: Failed to parse JSON: " + std::string(e.what()));
            return false;
        }

        // Log the raw contents (Step 2 - just print what we parsed)
        LogInfo("SceneLoader: Loaded scene: " + sceneData.dump(4));

        // Step 3 - create entities from the scene data
        if (sceneData.contains("entities"))
        {
            for (const auto& entityData : sceneData["entities"])
            {
                entt::entity entity = world.createEntity();
                LogInfo("SceneLoader: Created entity: " + entityData.value("name", std::string("unnamed")));
            }
        }

        return true;
    }
}