cmake_minimum_required(VERSION 3.15)
project(Strype-Editor VERSION 0.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_DIR "src/**.cpp" "src/**.h")
add_executable(Strype-Editor ${SOURCE_DIR})

set(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Strype-Editor PRIVATE Strype)
target_include_directories(Strype-Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(Strype-Editor Strype-Runtime)

add_custom_command(TARGET Strype-Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:Strype-Runtime>"
        "$ENV{STRYPE_DIR}/Strype/master"
)