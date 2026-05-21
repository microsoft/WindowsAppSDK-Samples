#----------------------------------------------------------------------------------------------------------------------
# MetapackageFrameworkDependent - Framework-dependent deployment using the Microsoft.WindowsAppSDK metapackage
#
# Demonstrates consuming WinAppSDK via the single metapackage NuGet rather than listing individual component
# packages. The metapackage pulls in all component packages transitively, but find_package() must still be
# called per component since each defines its own CMake targets.
#----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.31)

cmake_policy(SET CMP0141 NEW)
cmake_policy(SET CMP0117 NEW)

if(CMAKE_GENERATOR MATCHES "^Ninja")
    if(NOT (DEFINED ENV{Platform}))
        message(FATAL_ERROR "When using the Ninja generator, you must build from a platform-specific Developer Command Prompt.")
    endif()
endif()

project(MetapackageFrameworkDependent LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    NuGetCMakePackage
    GIT_REPOSITORY https://github.com/mschofie/NuGetCMakePackage
    GIT_TAG develop
)

FetchContent_MakeAvailable(NuGetCMakePackage)

# Only the metapackage is listed — it transitively pulls all component packages.
# CppWinRT and WebView2 are still listed explicitly as they are not part of the metapackage.
add_nuget_packages(
    CONFIG_FILE ${CMAKE_SOURCE_DIR}/../nuget.config
    LOCK_FILE ${CMAKE_SOURCE_DIR}/packages.lock.json
    FRAMEWORK native
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.250303.1
        Microsoft.WindowsAppSDK 2.0.0-experimental7
        Microsoft.Web.WebView2 1.0.3719.77
)

# Each component's find_package is still needed — the metapackage has no unified cmake config.
# The component NuGet packages were downloaded transitively and their cmake configs are probed individually.
find_package(Microsoft.WindowsAppSDK.Base CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Foundation CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.DWrite CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Widgets CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.AI CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.ML CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.WinUI CONFIG REQUIRED)

# Common configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN UNICODE _UNICODE)

add_executable(MetapackageFrameworkDependent WIN32
    main.cpp
)

set_target_properties(MetapackageFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

target_link_libraries(MetapackageFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_Framework
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite_Framework
        Microsoft.WindowsAppSDK.Widgets_Framework
        Microsoft.WindowsAppSDK.AI_Framework
        Microsoft.WindowsAppSDK.ML_Framework
        Microsoft.WindowsAppSDK.WinUI_Framework
)

# Copy runtime DLLs to the output directory
macro(post_build_runtime_dll_copy target_name)
    if(WIN32)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND};-E;$<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${target_name}>>,copy;$<TARGET_RUNTIME_DLLS:${target_name}>;$<TARGET_FILE_DIR:${target_name}>,true>"
            COMMAND_EXPAND_LISTS
        )
    endif()
endmacro()

post_build_runtime_dll_copy(MetapackageFrameworkDependent)

# Generate AppxManifest.xml with framework PackageDependency auto-injected
wasdk_generate_appx_manifest(
    TARGET MetapackageFrameworkDependent
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/AppxManifest.xml"
    IDENTITY_NAME "CMake-MetapackageFrameworkDependent"
    IDENTITY_PUBLISHER "CN=TestPublisher"
    IDENTITY_VERSION "1.0.0.0"
    DISPLAY_NAME "MetapackageFrameworkDependent (CMake)"
    DESCRIPTION "Packaged framework-dependent CMake test using metapackage"
)

# Copy Logo to the output directory for MSIX registration
add_custom_command(TARGET MetapackageFrameworkDependent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:MetapackageFrameworkDependent>
)
