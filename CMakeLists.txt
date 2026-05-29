cmake_minimum_required(VERSION 3.20)

project(project_v_engine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the core submodule as a subdirectory so we can reuse its targets.
add_subdirectory(external/project-v-core)

# Engine library (will depend on engine_core from the core repo).
add_library(projectv_engine STATIC)

target_sources(projectv_engine
    PRIVATE
        src/engine/editorApplication.cpp
)

target_link_libraries(projectv_engine
    PUBLIC
        engine_core   # core target defined in project-v-core's CMake
)

target_include_directories(projectv_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)