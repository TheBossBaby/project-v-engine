#pragma once
#include <string>
#include "projectV/engine/graph/scene/ecs/world.hpp"

namespace projectv::engine
{
    class SceneLoader
    {
    public:
        /**
         * @brief Load a scene from a JSON file and populate the world.
         * @param path Path to the .json scene file.
         * @param world The World instance to populate with entities.
         * @return true if loading succeeded, false otherwise.
         */
        bool loadFromFile(const std::string& path, World& world);
    };
}