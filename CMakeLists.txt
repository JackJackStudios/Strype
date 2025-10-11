cmake_minimum_required(VERSION 3.15)
project(Strype VERSION 0.4)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS  OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(AGI_EXAMPLES OFF CACHE BOOL "" FORCE)
set(AGI_EXTENSIONS ON CACHE BOOL "" FORCE)

add_compile_definitions(
    $<$<PLATFORM_ID:Windows>:STY_WINDOWS>
    $<$<PLATFORM_ID:Linux>:STY_LINUX>
    $<$<PLATFORM_ID:Darwin>:STY_MACOSX>

    $<$<CONFIG:Debug>:STY_DEBUG>
    $<$<CONFIG:Release>:STY_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:STY_RELEASE>
    $<$<CONFIG:MinSizeRel>:STY_DIST>
    YAML_CPP_STATIC_DEFINE
    
    STY_VERSION="${PROJECT_VERSION}"
)

set(ENV{STRYPE_DIR} ${CMAKE_SOURCE_DIR})

add_subdirectory(Strype)
add_subdirectory(Strype-Editor)
add_subdirectory(Strype-Runtime)

# Libraries
FetchContent_Declare(
    AGI
    GIT_REPOSITORY https://github.com/JackKnox/agi.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(AGI)

FetchContent_Declare(
    agi_extensions
    GIT_REPOSITORY https://github.com/JackJackStudios/agi_extensions.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(agi_extensions)

FetchContent_Declare(
    YAML_CPP
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(YAML_CPP)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
    Coral
    GIT_REPOSITORY https://github.com/JackKnox/Coral.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(Coral)

FetchContent_Declare(
    ImGuizmo
    GIT_REPOSITORY https://github.com/JackKnox/ImGuizmo.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(ImGuizmo)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(miniaudio)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-2
)
FetchContent_MakeAvailable(freetype)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG        v3.1.1
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(spdlog)