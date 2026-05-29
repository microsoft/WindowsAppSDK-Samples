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
        Microsoft.Windows.AI.MachineLearning 2.1.1
        Microsoft.Windows.CppWinRT 2.0.240405.15
        Microsoft.Windows.ImplementationLibrary 1.0.240803.1
        Microsoft.WindowsAppSDK.Foundation 2.0.21
        Microsoft.WindowsAppSDK.InteractiveExperiences 2.0.13
        Microsoft.WindowsAppSDK.ML 2.1.1
        Microsoft.WindowsAppSDK.Runtime 2.1.3
)

find_package(Microsoft.Windows.ImplementationLibrary CONFIG REQUIRED)

# Workaround: The ML cmake overlay expects the WinMD at Microsoft.WindowsAppSDK.ML/metadata/ but in 2.1.1+
# the WinMD lives in the separate Microsoft.Windows.AI.MachineLearning package. Copy the required files so
# the overlay can find them. Also copy the runtimes dir with onnxruntime.lib for the SelfContained target.
get_property(_ML_PKG_LOC GLOBAL PROPERTY NUGET_LOCATION-MICROSOFT_WINDOWSAPPSDK_ML)
get_property(_AIML_PKG_LOC GLOBAL PROPERTY NUGET_LOCATION-MICROSOFT_WINDOWS_AI_MACHINELEARNING)
if(NOT _ML_PKG_LOC)
    message(FATAL_ERROR "Microsoft.WindowsAppSDK.ML package location not found. Ensure the package is listed in add_nuget_packages().")
endif()
if(NOT _AIML_PKG_LOC)
    message(FATAL_ERROR "Microsoft.Windows.AI.MachineLearning package location not found. Ensure the package is listed in add_nuget_packages().")
endif()
if(NOT EXISTS "${_ML_PKG_LOC}/metadata")
    file(MAKE_DIRECTORY "${_ML_PKG_LOC}/metadata")
    file(COPY_FILE
        "${_AIML_PKG_LOC}/metadata/Microsoft.Windows.AI.MachineLearning.winmd"
        "${_ML_PKG_LOC}/metadata/Microsoft.Windows.AI.MachineLearning.winmd"
    )
    # Copy runtimes and lib for SelfContained target
    file(MAKE_DIRECTORY "${_ML_PKG_LOC}/runtimes/win-x64/native")
    file(MAKE_DIRECTORY "${_ML_PKG_LOC}/runtimes/win-arm64/native")
    file(COPY "${_AIML_PKG_LOC}/runtimes/win-x64/native/" DESTINATION "${_ML_PKG_LOC}/runtimes/win-x64/native/")
    file(COPY "${_AIML_PKG_LOC}/runtimes/win-arm64/native/" DESTINATION "${_ML_PKG_LOC}/runtimes/win-arm64/native/")
    file(COPY "${_AIML_PKG_LOC}/lib/native/x64/onnxruntime.lib" DESTINATION "${_ML_PKG_LOC}/runtimes/win-x64/native/")
    file(COPY "${_AIML_PKG_LOC}/lib/native/arm64/onnxruntime.lib" DESTINATION "${_ML_PKG_LOC}/runtimes/win-arm64/native/")
    # Also copy runtimes-framework dir
    file(MAKE_DIRECTORY "${_ML_PKG_LOC}/runtimes-framework/win-x64/native")
    file(MAKE_DIRECTORY "${_ML_PKG_LOC}/runtimes-framework/win-arm64/native")
    file(COPY "${_AIML_PKG_LOC}/runtimes/win-x64/native/" DESTINATION "${_ML_PKG_LOC}/runtimes-framework/win-x64/native/")
    file(COPY "${_AIML_PKG_LOC}/runtimes/win-arm64/native/" DESTINATION "${_ML_PKG_LOC}/runtimes-framework/win-arm64/native/")
endif()

# Workaround: InteractiveExperiences overlay expects lib at lib/native/win10-x64/ but package has lib/native/x64/
get_property(_IE_PKG_LOC GLOBAL PROPERTY NUGET_LOCATION-MICROSOFT_WINDOWSAPPSDK_INTERACTIVEEXPERIENCES)
if(_IE_PKG_LOC AND NOT EXISTS "${_IE_PKG_LOC}/lib/native/win10-x64")
    file(MAKE_DIRECTORY "${_IE_PKG_LOC}/lib/native/win10-x64")
    file(MAKE_DIRECTORY "${_IE_PKG_LOC}/lib/native/win10-arm64")
    file(COPY "${_IE_PKG_LOC}/lib/native/x64/" DESTINATION "${_IE_PKG_LOC}/lib/native/win10-x64/")
    file(COPY "${_IE_PKG_LOC}/lib/native/arm64/" DESTINATION "${_IE_PKG_LOC}/lib/native/win10-arm64/")
endif()

find_package(Microsoft.WindowsAppSDK.ML CONFIG REQUIRED)

# The ORT/WinML headers live in the Microsoft.Windows.AI.MachineLearning package
target_include_directories(Microsoft.WindowsAppSDK.ML
    INTERFACE
        "${_AIML_PKG_LOC}/include"
)

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
