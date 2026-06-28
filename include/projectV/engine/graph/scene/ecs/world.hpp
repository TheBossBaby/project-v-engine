#pragma once

#include <entt/entt.hpp>

namespace projectv::engine
{
    /**
     * @brief ECS world that owns the entity registry.
     *
     * This class provides a simple façade over entt::registry for
     * creating entities and attaching components.
     */
    class World
    {
        public:
        /**
         * @brief Create a new empty entity.
         *
         * @return The created entity identifier.
         */
        entt::entity createEntity();

        /**
         * @brief Destroy an existing entity.
         *
         * @param entity Entity to destroy.
         */
        void destroyEntity(entt::entity entity);

        /**
         * @brief Add a templated component to an entity.
         *
         * @param entity Entity to modify.
         * @param component Initial component data.
         */
        template<typename T>
        void addComponent(entt::entity entity, const T& component)
        {
            registry.emplace<T>(entity, component);
        }

        template<typename... Components>
        auto view() {
            return registry.view<Components...>();
        }
        
        private:
        /// Underlying ECS registry that stores all entities and components.
        entt::registry registry;
    };
}