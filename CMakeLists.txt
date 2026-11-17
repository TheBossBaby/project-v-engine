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

# === stb (header‑only image loading) ===
# The stb submodule lives in external/stb. Only stb_image.h is used; its
# implementation is compiled in src/engine/StbTextureLoader.cpp.
add_library(stb INTERFACE)

target_include_directories(stb INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
)

add_library(stb::stb ALIAS stb)
# === end stb ===

# === KTX-Software (.ktx2 textures) ===
# Fetched at configure time and pinned to a release tag. Provides libktx
# (static, for loading .ktx2 at runtime) and the `ktx` CLI (target
# `ktxtools`), which cmake/CookTextures.cmake runs at build time to cook
# source images into .ktx2. KTX-Software itself needs CMake 3.22 or newer.
include(FetchContent)

FetchContent_Declare(ktx
    GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
    GIT_TAG        v4.4.2
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES ""      # its only submodule is the conformance test suite
)

# Configured inside a function so these settings stay local to KTX and
# do not leak into the engine or the CMake cache.
function(projectv_add_ktx)
    set(KTX_FEATURE_TOOLS         ON)
    set(KTX_FEATURE_TESTS         OFF)
    set(KTX_FEATURE_TOOLS_CTS     OFF)
    set(KTX_FEATURE_DOC           OFF)
    set(KTX_FEATURE_LOADTEST_APPS OFF)
    set(KTX_FEATURE_JNI           OFF)
    set(KTX_FEATURE_PY            OFF)
    set(KTX_FEATURE_GL_UPLOAD     OFF)   # Vulkan renderer only
    set(KTX_FEATURE_ETC_UNPACK    OFF)
    set(BASISU_SUPPORT_OPENCL     OFF)
    set(BUILD_SHARED_LIBS         OFF)   # static libktx, so ktx.exe needs no DLL

    # KTX is C++17 code; under the engine's C++20, {fmt}'s compile-time
    # format checks reject the ktx tool's sources.
    set(CMAKE_CXX_STANDARD 17)

    # Unoptimised, the ktx tool cooks about 8x slower, so KTX is optimised
    # in Debug too. Only the optimisation flags change: KTX defines DEBUG
    # and _DEBUG in Debug and produces broken output if NDEBUG is added.
    foreach(lang C CXX)
        string(REGEX REPLACE "[/-]DNDEBUG" "" flags "${CMAKE_${lang}_FLAGS_RELWITHDEBINFO}")
        set(CMAKE_${lang}_FLAGS_DEBUG "${flags}")
    endforeach()

    # Git LFS in this repo only holds test images and OpenCL libraries.
    set(lfs_skip_smudge "$ENV{GIT_LFS_SKIP_SMUDGE}")
    set(ENV{GIT_LFS_SKIP_SMUDGE} 1)
    FetchContent_MakeAvailable(ktx)
    set(ENV{GIT_LFS_SKIP_SMUDGE} "${lfs_skip_smudge}")

    # KTX_FEATURE_TOOLS also adds toktx, ktxsc, ktxinfo, etc. Build only
    # the KTX targets something depends on (ktxtools for cooking).
    set_property(DIRECTORY ${ktx_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL TRUE)
endfunction()
projectv_add_ktx()
# === end KTX-Software ===

# Build-time texture cooking: defines pv_cook_textures() and the
# PV_TEXTURE_PLATFORM / PV_COOK_THREADS options. CMake functions are
# global, so a project that adds the engine (e.g. the game) can call
# pv_cook_textures() for its own executable and assets.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CookTextures.cmake)

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
        include/projectV/engine/resource/StbTextureLoader.hpp
        include/projectV/engine/resource/textureManager.hpp
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
        src/engine/StbTextureLoader.cpp
        src/engine/textureManager.cpp
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
        stb::stb           # texture loading is engine‑internal
)

set_target_properties(projectv_engine PROPERTIES LINKER_LANGUAGE CXX)

if(PROJECTV_BUILD_EDITOR)
    add_subdirectory(apps/editor)
else()
    add_subdirectory(apps/application)
endif()