#----------------------------------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.31)

cmake_policy(SET CMP0141 NEW) # MSVC debug information format flags are selected by an abstraction.
cmake_policy(SET CMP0117 NEW) # MSVC RTTI flag /GR is not added to CMAKE_CXX_FLAGS by default.

if(CMAKE_GENERATOR MATCHES "^Visual Studio")
    # To prevent builds (including try_compile) from picking up top-level 'Directory.Build.*' files,
    # write versions specific to the CMake builds at the root of the CMake build.
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.rsp" "-m -graphBuild:true -p:UseMultiToolTask=true -nodeReuse:false -terminalLogger:auto")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.props" "<Project></Project>")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.targets" "<Project></Project>")
elseif(CMAKE_GENERATOR MATCHES "^Ninja")
    if(NOT (DEFINED ENV{Platform}))
        message(FATAL_ERROR "When using the Ninja generator, you must build from a platform-specific Developer Command Prompt.")
    endif()
endif()

project(WinMLSamples LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  NuGetCMakePackage
  GIT_REPOSITORY https://github.com/mschofie/NuGetCMakePackage
  GIT_TAG b13e013481605d134f498fd9fc3a123c416da197
)

FetchContent_MakeAvailable(NuGetCMakePackage)

add_nuget_packages(
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.240405.15
        Microsoft.Windows.ImplementationLibrary 1.0.240803.1
        Microsoft.WindowsAppSDK.Foundation 1.8.250906002
        Microsoft.WindowsAppSDK.InteractiveExperiences 1.8.250906004
        Microsoft.WindowsAppSDK.ML 1.8.2091
        Microsoft.WindowsAppSDK.Runtime 1.8.250916003
)

find_package(Microsoft.Windows.ImplementationLibrary CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.ML CONFIG REQUIRED)

set(RESNET_MODEL_FILES
    ${CMAKE_SOURCE_DIR}/../Resources/ResNet50/model.Labels.txt
    ${CMAKE_SOURCE_DIR}/../Resources/ResNet50/model.LICENSE.txt
    ${CMAKE_SOURCE_DIR}/../Resources/ResNet50/model.onnx
    ${CMAKE_SOURCE_DIR}/../Resources/ResNet50/model.onnx.data
)

set(RESOURCE_FILES
    ${CMAKE_SOURCE_DIR}/Resources/dog.jpg
)

macro(post_build_runtime_dll_copy target_name)
    if(WIN32)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND};-E;$<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${target_name}>>,copy;$<TARGET_RUNTIME_DLLS:${target_name}>;$<TARGET_FILE_DIR:${target_name}>,true>"
            COMMAND_EXPAND_LISTS
        )
    endif()
endmacro()

# Common configuration
#

# Enable CMake support for Generators to include folders in their generated build output.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY APPEND PROPERTY VS_SOLUTION_ITEMS
    .clangd
    .editorconfig
    .gitignore
    CMakePresets.json
    readme.md
)

include(./Configuration.cmake)

link_libraries(Configuration)

# Subdirectories
#
add_subdirectory(ResNetCommon)
add_subdirectory(ResNetConsoleDesktop)
add_subdirectory(ResNetConsoleDesktop.SelfContained)
