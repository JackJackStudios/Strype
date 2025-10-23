file(GLOB_RECURSE SOURCE_DIR "src/**.cpp" "src/**.h")
add_executable(Strype-Runtime ${SOURCE_DIR})

target_link_libraries(Strype-Runtime PRIVATE Strype)
target_include_directories(Strype-Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# TODO: Remove! (Do one for editor, editor keeps passing same file across)
add_custom_command(TARGET Strype-Runtime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            $ENV{STRYPE_DIR}/Strype/master/DotNet/
            $<TARGET_FILE_DIR:Strype-Runtime>/DotNet/
)