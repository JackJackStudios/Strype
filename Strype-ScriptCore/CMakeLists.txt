file(COPY ${Coral_SOURCE_DIR} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

# Locate MSBuild
find_program(MSBUILD_PATH
    NAMES MSBuild.exe
    PATHS
        "$ENV{ProgramFiles}/Microsoft Visual Studio"
        "$ENV{ProgramFiles}/MSBuild"
        "$ENV{ProgramFilesx86}/Microsoft Visual Studio"
    PATH_SUFFIXES
        "2022/Community/MSBuild/Current/Bin"
        "2019/Community/MSBuild/Current/Bin"
        "MSBuild/Current/Bin"
)

add_custom_target(Strype-ScriptCore
    COMMAND "C:/Program Files/Microsoft Visual Studio/2022/Community/Msbuild/Current/Bin/MSBuild.exe" Strype-ScriptCore.csproj -property:Configuration=Debug -t:restore,build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building Strype-ScriptCore"
)

add_dependencies(Strype-ScriptCore Coral.Managed)