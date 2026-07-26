#include "projectV/engine/resource/sceneLoader.hpp"
#include "projectV/engine/logging/log.hpp"
#include <projectV/core/transform.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

namespace projectv::engine
{
    bool SceneLoader::loadFromFile(const std::string& path, World& world)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            LogError("SceneLoader: Could not open file: " + path);
            return false;
        }

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

        LogInfo("SceneLoader: Loaded scene: " + sceneData.dump(4));

        if (sceneData.contains("entities"))
        {
            for (const auto& entityData : sceneData["entities"])
            {
                entt::entity entity = world.createEntity();
                LogInfo("SceneLoader: Created entity: " + entityData.value("name", std::string("unnamed")));

                // Transform component
                if (entityData.contains("transform"))
                {
                    const auto& t = entityData["transform"];
                    core::Transform transform;

                    if (t.contains("position") && t["position"].size() == 3)
                        transform.position = { t["position"][0], t["position"][1], t["position"][2] };

                    if (t.contains("rotation") && t["rotation"].size() == 3)
                        transform.eulerDegrees = { t["rotation"][0], t["rotation"][1], t["rotation"][2] };

                    if (t.contains("scale") && t["scale"].size() == 3)
                        transform.scale = { t["scale"][0], t["scale"][1], t["scale"][2] };

                    world.addComponent<core::Transform>(entity, transform);
                    LogInfo("SceneLoader: Attached Transform to entity: " + entityData.value("name", std::string("unnamed")));
                }
            }
        }

        return true;
    }
}