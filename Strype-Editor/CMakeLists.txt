file(GLOB_RECURSE SOURCE_DIR "src/**.cpp")
add_executable(Strype-Editor ${SOURCE_DIR})

target_link_libraries(Strype-Editor PRIVATE Strype)
target_include_directories(Strype-Editor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_custom_command(TARGET Strype-Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Strype-Editor>/assets/
)

# TODO: Remove! (Do one for editor, editor keeps passing same file across)
add_custom_command(TARGET Strype-Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            $ENV{STRYPE_DIR}/Strype/master/DotNet/
            $<TARGET_FILE_DIR:Strype-Editor>/DotNet/
)

add_dependencies(Strype-Editor Strype-Runtime)