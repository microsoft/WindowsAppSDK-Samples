#----------------------------------------------------------------------------------------------------------------------
# CMakeBaselineTests - WinAppSDK Deployment Matrix Baseline Tests (CMake + NuGetCMakePackage)
#----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.31)

cmake_policy(SET CMP0141 NEW)
cmake_policy(SET CMP0117 NEW)

if(CMAKE_GENERATOR MATCHES "^Visual Studio")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.rsp" "-m -graphBuild:true -p:UseMultiToolTask=true -nodeReuse:false -terminalLogger:auto")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.props" "<Project></Project>")
    file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.targets" "<Project></Project>")
elseif(CMAKE_GENERATOR MATCHES "^Ninja")
    if(NOT (DEFINED ENV{Platform}))
        message(FATAL_ERROR "When using the Ninja generator, you must build from a platform-specific Developer Command Prompt.")
    endif()
endif()

project(CMakeBaselineTests LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    NuGetCMakePackage
    GIT_REPOSITORY https://github.com/mschofie/NuGetCMakePackage
    GIT_TAG user/ssparach/prototype
)

FetchContent_MakeAvailable(NuGetCMakePackage)

add_nuget_packages(
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.251203.1
        Microsoft.WindowsAppSDK.Foundation 2.0.15-preview
        Microsoft.WindowsAppSDK.InteractiveExperiences 2.0.6-preview
        Microsoft.WindowsAppSDK.Runtime 2.0.0-preview1
)

find_package(Microsoft.WindowsAppSDK.Foundation CONFIG REQUIRED)

# Common configuration
include(${CMAKE_SOURCE_DIR}/Configuration.cmake)
link_libraries(Configuration)

# Macro to copy runtime DLLs to the output directory
macro(post_build_runtime_dll_copy target_name)
    if(WIN32)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND};-E;$<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${target_name}>>,copy;$<TARGET_RUNTIME_DLLS:${target_name}>;$<TARGET_FILE_DIR:${target_name}>,true>"
            COMMAND_EXPAND_LISTS
        )
    endif()
endmacro()

# Subdirectories
add_subdirectory(UnpackagedSelfContained)
add_subdirectory(UnpackagedFrameworkDependent)
add_subdirectory(PackagedSelfContained)
add_subdirectory(PackagedFrameworkDependent)
