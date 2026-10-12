#----------------------------------------------------------------------------------------------------------------------
# Metapackage samples — shared root
#
# Demonstrates the new umbrella consumption shape for Microsoft.WindowsAppSDK across the four
# deployment-matrix combinations (Packaged/Unpackaged × Framework-dep/Self-contained):
#
#   - Single add_nuget_packages() entry for the metapackage. Every component the metapackage
#     transitively pulls in is automatically materialized.
#   - Single find_package(Microsoft.WindowsAppSDK CONFIG REQUIRED). The umbrella config
#     materializes every component.
#   - Each sample subdirectory is thin: project + add_executable + per-target properties
#     (WindowsPackageType, WindowsAppSDKSelfContained, auto-init switches) + single
#     target_link_libraries(... Microsoft.WindowsAppSDK).
#   - No per-component find_package or per-target *_Framework / *_SelfContained suffix linking.
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

project(MetapackageSamples LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    NuGetCMakePackage
    GIT_REPOSITORY https://github.com/mschofie/NuGetCMakePackage
    GIT_TAG develop
)

FetchContent_MakeAvailable(NuGetCMakePackage)

add_nuget_packages(
    CONFIG_FILE ${CMAKE_SOURCE_DIR}/../nuget.config
    LOCK_FILE ${CMAKE_SOURCE_DIR}/packages.lock.json
    FRAMEWORK native
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.251203.1
        Microsoft.WindowsAppSDK 2.0.0-experimental7
)

# Single find_package — the metapackage's umbrella config materializes every component.
find_package(Microsoft.WindowsAppSDK CONFIG REQUIRED)

# Common compiler configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN UNICODE _UNICODE)

#----------------------------------------------------------------------------------------------------------------------
# Post-build helpers
#
# wasdk_post_build_copy() and friends are provided by Microsoft.WindowsAppSDK.Base via
# WindowsAppSDKHelpers.cmake. Loaded transitively by every component find_package call,
# so consumer CMakeLists files do not need to define any post-build plumbing themselves.
#----------------------------------------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------------------------------------
# Sub-samples (one per deployment matrix cell)
#----------------------------------------------------------------------------------------------------------------------
add_subdirectory(PackagedFrameworkDependent)
add_subdirectory(PackagedSelfContained)
add_subdirectory(UnpackagedFrameworkDependent)
add_subdirectory(UnpackagedSelfContained)
