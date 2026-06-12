cmake_minimum_required(VERSION 3.20)

project(project_v_engine
    VERSION 0.1.0
    LANGUAGES CXX
)

option(PROJECTV_BUILD_EDITOR "Build the Editor executable instead of Application" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Turn off GLFW extras to keep builds fast
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Core + third‑party deps
add_subdirectory(external/project-v-core)
add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)

# Mesh Loading
add_subdirectory(external/assimp)

# === EnTT (header‑only ECS) ===
# The EnTT submodule lives in external/entt.
# We create a small INTERFACE target and expose its include directory,
# then link that into projectv_engine so we can #include <entt/entt.hpp>.
add_library(EnTT INTERFACE)

target_include_directories(EnTT INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/entt/src
)

add_library(EnTT::EnTT ALIAS EnTT)
# === end EnTT ===

# engine lib setup here ...
add_library(projectv_engine STATIC)

# Add sources
target_sources(projectv_engine
    PUBLIC
        include/projectV/engine/logging/log.hpp
        include/projectV/engine/logging/SpdLogger.hpp

        include/projectV/engine/core/application.hpp

        include/projectV/engine/platform/glfwWindow.hpp

        include/projectv/engine/resource/meshLoaderAssimp.hpp
        include/projectv/engine/resource/meshManager.hpp
        include/projectv/engine/resource/shaderManager.hpp
    PRIVATE
        src/engine/log.cpp
        src/engine/SpdLogger.cpp

        src/engine/application.cpp
        src/engine/glfwWindow.cpp

        src/engine/meshLoaderAssimp.cpp
        src/engine/meshManager.cpp
        src/engine/shaderManager.cpp         
)

# Public headers
target_include_directories(projectv_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link core + third‑party libs
target_link_libraries(projectv_engine
    PUBLIC
        engine_core
        spdlog::spdlog
        glfw
        EnTT::EnTT         # <- EnTT available to anything using projectv_engine
    PRIVATE
        assimp             # mesh loading is engine‑internal
)

set_target_properties(projectv_engine PROPERTIES LINKER_LANGUAGE CXX)

if(PROJECTV_BUILD_EDITOR)
    add_subdirectory(apps/editor)
else()
    add_subdirectory(apps/application)
endif()