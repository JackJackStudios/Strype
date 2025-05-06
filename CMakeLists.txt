cmake_minimum_required(VERSION 3.15)
project(Strype VERSION 0.4)

include(FetchContent)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_link_options("/ignore:4099")

add_compile_definitions(
    $<$<CONFIG:Debug>:STY_DEBUG>
    $<$<CONFIG:Release>:STY_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:STY_RELEASE>
    $<$<CONFIG:MinSizeRel>:STY_DIST>
    YAML_CPP_STATIC_DEFINE
)

set(ENV{STRYPE_DIR} ${CMAKE_SOURCE_DIR})

set(STRYPE_LIBRARIES "${CMAKE_SOURCE_DIR}/Libraries")
message(STATUS "Library directory: ${STRYPE_LIBRARIES}")

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
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d.git
  GIT_TAG        v3.1.0
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
  EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.15.0
)
FetchContent_MakeAvailable(EnTT)

FetchContent_Declare(
  GLFW
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)
FetchContent_MakeAvailable(GLFW)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/icaven/glm.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glm)

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
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  Coral
  GIT_REPOSITORY https://github.com/JackKnox/Coral.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(Coral)

FetchContent_Declare(
  ImGui
  GIT_REPOSITORY https://github.com/JackKnox/imgui.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(ImGui)