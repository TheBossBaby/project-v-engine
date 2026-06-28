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
}
