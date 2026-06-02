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

add_subdirectory(external/project-v-core)
add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)

# engine lib setup here ...
add_library(projectv_engine STATIC)

# Add sources
target_sources(projectv_engine
    PUBLIC
        include/projectV/engine/log.hpp
        include/projectV/engine/SpdLogger.hpp

        include/projectV/engine/application.hpp
        include/projectV/engine/glfwWindow.hpp
    PRIVATE
        src/engine/log.cpp
        src/engine/SpdLogger.cpp

        src/engine/application.cpp
        src/engine/glfwWindow.cpp
)

# Public headers
target_include_directories(projectv_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(projectv_engine PUBLIC engine_core)
target_link_libraries(projectv_engine PUBLIC spdlog::spdlog)
target_link_libraries(projectv_engine PUBLIC glfw)

set_target_properties(projectv_engine PROPERTIES LINKER_LANGUAGE CXX)


if(PROJECTV_BUILD_EDITOR)
    add_subdirectory(apps/editor)
else()
    add_subdirectory(apps/application)
endif()