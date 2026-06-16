#include <projectV/core/transform.hpp>
#include <projectV/core/meshHandle.hpp>

#include <projectV/engine/graph/scene/ecs/world.hpp>
#include <projectV/engine/logging/log.hpp>

#include<string>

namespace projectv::engine
{
    entt::entity World::createEntity()
    {
        return registry.create();
    }

    void World::destroyEntity(entt::entity entity)
    {
        registry.destroy(entity);
    }

    void World::logRenderableEntities() const
    {
        auto view = registry.view<
            const projectv::core::Transform,
            const projectv::core::MeshHandle>();

        for (auto entity : view)
        {
            const auto& transform    = view.get<projectv::core::Transform>(entity);
            const auto& meshHandle   = view.get<projectv::core::MeshHandle>(entity);

            std::string logMessage =
            "World: entity="
                    + std::to_string(static_cast<std::uint32_t>(entity))
                    + " meshIndex=" + std::to_string(meshHandle.index)
                    + " position=("
                    + std::to_string(transform.position.x) + ", "
                    + std::to_string(transform.position.y) + ", "
                    + std::to_string(transform.position.z) + ")";
            
            LogInfo(logMessage);
        }
    }
}
