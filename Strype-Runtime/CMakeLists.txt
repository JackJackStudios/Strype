cmake_minimum_required(VERSION 3.15)
project(Strype-Runtime VERSION 0.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_DIR "src/**.cpp" "src/**.h")
add_executable(Strype-Runtime ${SOURCE_DIR})

set(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Strype-Runtime PRIVATE Strype)
target_include_directories(Strype-Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})