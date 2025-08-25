file(GLOB_RECURSE SOURCE_DIR "src/**.cpp" "src/**.hpp")
add_library(Strype STATIC ${SOURCE_DIR})

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb/
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)
FetchContent_GetProperties(stb)

target_precompile_headers(Strype PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/stypch.hpp")

target_include_directories(Strype PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(Strype PUBLIC ${stb_SOURCE_DIR})

target_link_libraries(Strype PUBLIC
    agi
    agi_imgui

    glm
    box2d
    ImGuizmo
    miniaudio
    spdlog::spdlog
    yaml-cpp::yaml-cpp
    magic_enum::magic_enum
    Coral
)

# Download scripting binaries
file(DOWNLOAD
    "https://github.com/JackJackStudios/Strype-ScriptCore/releases/download/latest/Coral.Managed.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/master/DotNet/Coral.Managed.dll"
    SHOW_PROGRESS
    STATUS status)

file(DOWNLOAD
    "https://github.com/JackJackStudios/Strype-ScriptCore/releases/download/latest/Strype-ScriptCore.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/master/DotNet/Strype-ScriptCore.dll"
    SHOW_PROGRESS
    STATUS status)