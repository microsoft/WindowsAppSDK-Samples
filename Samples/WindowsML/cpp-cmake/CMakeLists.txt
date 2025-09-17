#----------------------------------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.31)

cmake_policy(SET CMP0141 NEW) # MSVC debug information format flags are selected by an abstraction.
cmake_policy(SET CMP0117 NEW) # MSVC RTTI flag /GR is not added to CMAKE_CXX_FLAGS by default.

if(CMAKE_GENERATOR MATCHES "^Visual Studio")
    # To prevent builds (including try_compile) from picking up top-level 'Directory.Build.*' files,
    # write versions specific to the CMake builds at the root of the CMake build.
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.rsp" "-m -graphBuild:True -p:UseMultiToolTask=True")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.props" "<Project></Project>")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.targets" "<Project></Project>")
elseif(CMAKE_GENERATOR MATCHES "^Ninja")
    if(NOT (DEFINED ENV{Platform}))
        message(FATAL_ERROR "When using the Ninja generator, you must build from a platform-specific Developer Commnand Prompt.")
    endif()
endif()

project(WinMLSamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_LIBRARIES "")
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)

include(FetchContent)

FetchContent_Declare(
  CMakeNuGetPackage
  GIT_REPOSITORY https://github.com/mschofie/NuGetCMakePackage
  GIT_TAG 74545405a30aa3e358665ea1431948f61875edad
)

FetchContent_MakeAvailable(CMakeNuGetPackage)

add_nuget_library(Microsoft.Windows.ImplementationLibrary 1.0.250325.1)
add_nuget_library(Microsoft.Windows.CppWinRT 2.0.250303.1)

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
include(./Configuration.cmake)

link_libraries(Configuration)

# Subdirectories
#
add_subdirectory(ResNetCommon)
add_subdirectory(ResNetConsoleDesktop)
add_subdirectory(ResNetConsoleDesktop.SelfContained)
