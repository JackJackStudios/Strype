file(GLOB_RECURSE SOURCE_DIR "src/**.cpp" "src/**.h")
add_executable(Strype-Editor ${SOURCE_DIR})

set(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Strype-Editor PRIVATE Strype)
target_include_directories(Strype-Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})