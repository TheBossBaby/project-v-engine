cmake_minimum_required(VERSION 3.20)

project(project_v_engine
    VERSION 0.1.0
    LANGUAGES CXX
)

option(PROJECTV_BUILD_EDITOR "Build the Editor executable instead of Application" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/project-v-core)

# engine lib setup here ...
add_library(projectv_engine STATIC ...)
target_link_libraries(projectv_engine PUBLIC engine_core)

set_target_properties(projectv_engine PROPERTIES LINKER_LANGUAGE CXX)


if(PROJECTV_BUILD_EDITOR)
    add_subdirectory(apps/editor)
else()
    add_subdirectory(apps/application)
endif()