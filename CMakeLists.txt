cmake_minimum_required(VERSION 3.10)
project(SDL2_Cube_Demo C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)

add_executable(cube_demo main.c)

target_include_directories(cube_demo PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(cube_demo PRIVATE ${SDL2_LIBRARIES})

# Link mathematical library explicitly for Unix environments
if(UNIX AND NOT APPLE)
    target_link_libraries(cube_demo PRIVATE m)
endif()
