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

add_nuget_packages(
    CONFIG_FILE ${CMAKE_SOURCE_DIR}/nuget.config
    LOCK_FILE ${CMAKE_SOURCE_DIR}/packages.lock.json
    FRAMEWORK native
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.250303.1
        Microsoft.WindowsAppSDK.Base 2.0.5-experimental2
        Microsoft.WindowsAppSDK.Foundation 2.0.19-experimental
        Microsoft.WindowsAppSDK.InteractiveExperiences 2.0.11-experimental
        Microsoft.WindowsAppSDK.DWrite 2.0.2-experimental
        Microsoft.WindowsAppSDK.Runtime 2.0.0-experimental7
        Microsoft.WindowsAppSDK.Widgets 2.0.4-experimental
        Microsoft.WindowsAppSDK.AI 2.0.179-experimental
        Microsoft.WindowsAppSDK.ML 2.0.325-experimental
        Microsoft.Web.WebView2 1.0.3719.77
        Microsoft.WindowsAppSDK.WinUI 2.0.10-experimental
)

find_package(Microsoft.WindowsAppSDK.Base CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Foundation CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.DWrite CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.Widgets CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.AI CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.ML CONFIG REQUIRED)
find_package(Microsoft.WindowsAppSDK.WinUI CONFIG REQUIRED)

# Common cmake configuration
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

# For pure WinRT components (Widgets, AI, WinUI) that have no import libraries (.lib).
# These components store DLL paths in a custom INTERFACE_SELFCONTAINED_RUNTIME_DLLS property instead. This macro
# reads that property and copies the DLLs via post-build commands.
macro(post_build_selfcontained_dll_copy target_name)
    foreach(_sc_target
        Microsoft.WindowsAppSDK.Widgets_SelfContained
        Microsoft.WindowsAppSDK.AI_SelfContained
        Microsoft.WindowsAppSDK.ML_SelfContained
        Microsoft.WindowsAppSDK.WinUI_SelfContained
    )
        if(TARGET ${_sc_target})
            get_property(_sc_dlls TARGET ${_sc_target} PROPERTY INTERFACE_SELFCONTAINED_RUNTIME_DLLS)
            if(_sc_dlls)
                foreach(_dll IN LISTS _sc_dlls)
                    add_custom_command(TARGET ${target_name} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_dll}" "$<TARGET_FILE_DIR:${target_name}>"
                    )
                endforeach()
            endif()
        endif()
    endforeach()
endmacro()

# Subdirectories
add_subdirectory(UnpackagedSelfContained)
add_subdirectory(UnpackagedFrameworkDependent)
add_subdirectory(PackagedSelfContained)
add_subdirectory(PackagedFrameworkDependent)


