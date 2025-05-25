file(GLOB_RECURSE SOURCE_DIR "src/**.cpp" "src/**.h")
add_library(Strype STATIC ${SOURCE_DIR})

target_precompile_headers(Strype PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/stypch.h")

target_include_directories(Strype PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(Strype PUBLIC "${STRYPE_LIBRARIES}/include")

target_link_libraries(Strype PUBLIC
    "${STRYPE_LIBRARIES}/lib/freetype.lib"
    "${STRYPE_LIBRARIES}/lib/OpenAL32.lib"
    "${STRYPE_LIBRARIES}/lib/sndfile.lib"

    AGI
    glm
    box2d
    glfw
    ImGui
    ImGuizmo
    Coral.Native
    EnTT::EnTT
    spdlog::spdlog
    yaml-cpp::yaml-cpp
    magic_enum::magic_enum
)

add_dependencies(Strype Strype-ScriptCore)