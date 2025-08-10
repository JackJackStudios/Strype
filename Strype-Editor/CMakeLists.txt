cmake_minimum_required(VERSION 3.15)
project(Strype-Editor VERSION 0.4)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_DIR "src/**.cpp")
add_executable(Strype-Editor ${SOURCE_DIR})

target_link_libraries(Strype-Editor PRIVATE Strype)
target_include_directories(Strype-Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets/
)

add_dependencies(Strype-Editor Strype-Runtime copy_assets)