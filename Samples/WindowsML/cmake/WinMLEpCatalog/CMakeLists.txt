# Copyright (C) Microsoft Corporation. All rights reserved.
cmake_minimum_required(VERSION 3.21)

project(WinMLEpCatalogSample
    VERSION 1.0.0
    DESCRIPTION "Windows ML Execution Provider Catalog Sample with ONNX Runtime Integration"
    LANGUAGES CXX
)

# NuGet package version
set(WINML_NUGET_ID      "Microsoft.WindowsAppSDK.ML" CACHE STRING "NuGet package ID")
set(WINML_NUGET_VERSION "1.8.2141"                   CACHE STRING "NuGet package version")

# Packages are restored / extracted under the build directory.
cmake_path(APPEND CMAKE_CURRENT_BINARY_DIR "packages" OUTPUT_VARIABLE WINML_PACKAGES_DIR)

# Repo-level NuGet config (contains the WinAppSDK-SampleDeps feed and localpackages).
cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "../../../../Samples/nuget.config"
    OUTPUT_VARIABLE _winml_nuget_cfg_default)
cmake_path(NORMAL_PATH _winml_nuget_cfg_default)
set(WINML_NUGET_CONFIG "${_winml_nuget_cfg_default}"
    CACHE FILEPATH "Path to NuGet.Config for package restore")

#  Auto-restore & extract
# Users can still skip all of this by setting microsoft.windows.ai.machinelearning_DIR.
if(NOT DEFINED microsoft.windows.ai.machinelearning_DIR)
    set(_winml_pkg_name  "${WINML_NUGET_ID}.${WINML_NUGET_VERSION}")
    cmake_path(APPEND WINML_PACKAGES_DIR "${_winml_pkg_name}" OUTPUT_VARIABLE _winml_extract_dir)
    cmake_path(APPEND _winml_extract_dir "build/cmake/microsoft.windows.ai.machinelearning-config.cmake"
        OUTPUT_VARIABLE _winml_config_file)

    if(NOT EXISTS "${_winml_config_file}")
        # Try a local .nupkg first (handy for pre-release testing).
        cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${_winml_pkg_name}.nupkg"
            OUTPUT_VARIABLE _winml_local_nupkg)
        if(EXISTS "${_winml_local_nupkg}")
            file(MAKE_DIRECTORY "${_winml_extract_dir}")
            message(STATUS "Extracting local ${_winml_local_nupkg}")
            file(ARCHIVE_EXTRACT INPUT "${_winml_local_nupkg}" DESTINATION "${_winml_extract_dir}")
        else()
            # Restore via nuget install using the repo's NuGet.Config.
            find_program(_NUGET_EXE nuget REQUIRED)
            cmake_path(ABSOLUTE_PATH WINML_NUGET_CONFIG NORMALIZE OUTPUT_VARIABLE _winml_nuget_cfg)
            message(STATUS "Restoring ${_winml_pkg_name} via nuget install (config: ${_winml_nuget_cfg}) ...")
            execute_process(
                COMMAND "${_NUGET_EXE}" install "${WINML_NUGET_ID}"
                        -Version "${WINML_NUGET_VERSION}"
                        -OutputDirectory "${WINML_PACKAGES_DIR}"
                        -ConfigFile "${_winml_nuget_cfg}"
                        -NonInteractive -DirectDownload
                        -DependencyVersion Ignore
                RESULT_VARIABLE _nuget_result
            )
            if(NOT _nuget_result EQUAL 0)
                message(FATAL_ERROR
                    "nuget install failed (exit ${_nuget_result}). "
                    "Place ${_winml_pkg_name}.nupkg in this directory or check your NuGet sources.")
            endif()
        endif()
    endif()

    if(NOT EXISTS "${_winml_config_file}")
        message(FATAL_ERROR "CMake config not found after restore: ${_winml_config_file}")
    endif()

    cmake_path(APPEND _winml_extract_dir "build/cmake" OUTPUT_VARIABLE _winml_cmake_dir)
    set(microsoft.windows.ai.machinelearning_DIR "${_winml_cmake_dir}"
        CACHE PATH "Path to microsoft.windows.ai.machinelearning CMake config" FORCE)
endif()

# Find the Windows ML package from the extracted NuGet package CMake config.
find_package(microsoft.windows.ai.machinelearning CONFIG REQUIRED)

# C++20 standard for modern features (std::format, etc.)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the executable
add_executable(WinMLEpCatalogSample
    main.cpp
)

# WindowsML::Api       - WinMLEpCatalog* C API for discovering execution providers
# WindowsML::OnnxRuntime - ONNX Runtime C/C++ API (OrtEnv, session creation, etc.)
target_link_libraries(WinMLEpCatalogSample
    PRIVATE
        WindowsML::Api
        WindowsML::OnnxRuntime
)

# Copy runtime DLLs (onnxruntime.dll, Microsoft.Windows.AI.MachineLearning.dll)
# to the executable output directory so the sample can run in-place.
add_custom_command(TARGET WinMLEpCatalogSample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:WinMLEpCatalogSample> $<TARGET_FILE_DIR:WinMLEpCatalogSample>
    COMMAND_EXPAND_LISTS
    VERBATIM
)

# DirectML.dll and onnxruntime_providers_shared.dll have no import libraries,
# so they are not included in TARGET_RUNTIME_DLLS. Copy them explicitly so
# execution provider registration works when the sample runs in-place.
add_custom_command(TARGET WinMLEpCatalogSample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WINML_DIRECTML_DLL}" $<TARGET_FILE_DIR:WinMLEpCatalogSample>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WINML_ONNXRUNTIME_PROVIDERS_SHARED_DLL}" $<TARGET_FILE_DIR:WinMLEpCatalogSample>
    COMMAND_EXPAND_LISTS
    VERBATIM
)

# Set Windows-specific compiler options
if(MSVC)
    target_compile_options(WinMLEpCatalogSample PRIVATE
        /W4             # Warning level 4
        /permissive-    # Strict C++ conformance
        /Zc:__cplusplus # Report correct __cplusplus value
        /EHsc           # Enable C++ exception handling
    )
    
    # Use static runtime for easier deployment (no MSVC runtime dependency)
    set_property(TARGET WinMLEpCatalogSample PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

# Set subsystem to console (not Windows GUI)
if(WIN32)
    set_target_properties(WinMLEpCatalogSample PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()
