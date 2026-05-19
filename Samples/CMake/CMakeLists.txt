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
    GIT_TAG develop
)

FetchContent_MakeAvailable(NuGetCMakePackage)

#----------------------------------------------------------------------------------------------------------------------
# These samples illustrate individual component usage — picking and choosing between the
# NuGet packages shipped as part of the WindowsAppSDK. If you want the entirety of the
# WindowsAppSDK pulled in via a single dependency, see the Metapackage sample for a
# better illustration of that use case.
#----------------------------------------------------------------------------------------------------------------------

add_nuget_packages(
    CONFIG_FILE ${CMAKE_SOURCE_DIR}/nuget.config
    LOCK_FILE ${CMAKE_SOURCE_DIR}/packages.lock.json
    FRAMEWORK native
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.251203.1
        Microsoft.WindowsAppSDK.Base 2.0.3-experimental1
        Microsoft.WindowsAppSDK.Foundation 2.0.19-experimental
        Microsoft.WindowsAppSDK.InteractiveExperiences 2.0.11-experimental
        Microsoft.WindowsAppSDK.DWrite 2.0.2-experimental
        Microsoft.WindowsAppSDK.Runtime 2.0.0-experimental7
        Microsoft.WindowsAppSDK.Widgets 2.0.4-experimental
        Microsoft.WindowsAppSDK.AI 2.0.179-experimental
        Microsoft.WindowsAppSDK.ML 2.0.325-experimental
        Microsoft.WindowsAppSDK.WinUI 2.0.10-experimental
)

find_package(Microsoft.WindowsAppSDK.Base CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Runtime CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Foundation CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.InteractiveExperiences CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.DWrite CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Widgets CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.AI CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.ML CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.WinUI CONFIG REQUIRED)

# Common cmake configuration
include(${CMAKE_SOURCE_DIR}/Configuration.cmake)
link_libraries(Configuration)

# Post-build helpers are provided by
# Microsoft.WindowsAppSDK.Base via WindowsAppSDKHelpers.cmake.

# Subdirectories
add_subdirectory(UnpackagedSelfContained)
add_subdirectory(UnpackagedFrameworkDependent)
add_subdirectory(PackagedSelfContained)
add_subdirectory(PackagedFrameworkDependent)


