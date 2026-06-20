#pragma once

#include "projectV/core/application.hpp"

namespace projectv::engine
{
    class EditorApplication : public projectv::core::IApplication
    {
    public:
        bool init(const projectv::core::WindowDesc& windowDesc) override;
        void run() override;
        void shutdown() override;

    protected:
        void tick(float deltaTime) override;
        void render() override;
    };
}