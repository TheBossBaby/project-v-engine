cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(project_v_app
    src/mainApp.cpp
)

target_link_libraries(project_v_app
    PRIVATE
        projectv_engine
)