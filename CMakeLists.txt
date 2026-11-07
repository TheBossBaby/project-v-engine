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

        include/projectV/engine/resource/levelLoader.hpp
        include/projectV/engine/resource/meshLoaderAssimp.hpp
        include/projectV/engine/resource/meshManager.hpp
        include/projectV/engine/resource/shaderManager.hpp
        include/projectV/engine/resource/fileShaderLoader.hpp
        
        include/projectV/engine/graph/scene/ecs/world.hpp

        include/projectV/engine/util/assetPath.hpp
        include/projectV/engine/util/assets.hpp
    PRIVATE
        src/engine/log.cpp
        src/engine/SpdLogger.cpp

        src/engine/application.cpp
        src/engine/glfwWindow.cpp

        src/engine/levelLoader.cpp
        src/engine/meshLoaderAssimp.cpp
        src/engine/meshManager.cpp
        src/engine/shaderManager.cpp      
        src/engine/fileShaderLoader.cpp

        src/engine/world.cpp  
)

# Public headers
target_include_directories(projectv_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Directory that contains the assets. It is passed to the code as the
# ASSETROOTLOCATION compile definition (see include/projectV/engine/util/assets.hpp),
# and asset paths in a LevelDesc are relative to it.
# The default is <top-level project>/assets. To use another folder, pass
# -DPROJECTV_ASSET_ROOT=<path> to CMake, or set the variable before add_subdirectory().
set(PROJECTV_ASSET_ROOT "${CMAKE_SOURCE_DIR}/assets"
    CACHE PATH "Directory that contains the assets used by the level loader")

if(NOT EXISTS "${PROJECTV_ASSET_ROOT}")
    message(WARNING "PROJECTV_ASSET_ROOT does not exist: ${PROJECTV_ASSET_ROOT}")
endif()

message(STATUS "Asset root: ${PROJECTV_ASSET_ROOT}")

# PUBLIC because assets.hpp is a public header that uses the definition.
target_compile_definitions(projectv_engine
    PUBLIC
        ASSETROOTLOCATION="${PROJECTV_ASSET_ROOT}"
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