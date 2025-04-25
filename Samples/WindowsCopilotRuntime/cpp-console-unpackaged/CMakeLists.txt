# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
cmake_minimum_required(VERSION 3.21)

project(cmake-ai-generator)

find_package(wil CONFIG REQUIRED)
find_package(cppwinrt CONFIG REQUIRED)
find_package(windowsappsdk CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(cmake-ai-generator
    main.cpp
    app.manifest
    formatters.h)

target_link_libraries(cmake-ai-generator
    INTERFACE
        WindowsAppSdk::Bootstrap
    PRIVATE
        Microsoft::CppWinRT
        Microsoft::WindowsAppSdk
        OneCoreUAP
        DelayImp
)

add_winappsdk(cmake-ai-generator)

# Delay loading this DLL ensures that the executable can _run_ on older Windows versions, but detect
# whether or not the necessary manual WinAppRuntime support is there.
target_link_options(cmake-ai-generator
    PRIVATE
    "/delayload:api-ms-win-appmodel-runtime-l1-1-5.dll")

function(install_target_runtime_dlls)
  set(single_value_args DESTINATION)
  set(multi_value_args TARGETS)
  cmake_parse_arguments(THIS "" "${single_value_args}" "${multi_value_args}" ${ARGN})

  foreach(TARGET ${THIS_TARGETS})
    get_target_property(TARGET_RUNTIME_DLLS ${TARGET} RUNTIME_DLLS)
    if ("${TARGET_RUNTIME_DLLS}" STREQUAL "")
      message(FATAL_ERROR "Provided target ${TARGET} has no RUNTIME_DLLS")
    endif()

    foreach(ONEFILE ${TARGET_RUNTIME_DLLS})
        file(COPY ${ONEFILE}
          DESTINATION ${THIS_DESTINATION}
        )
        endforeach()
    endforeach()
endfunction()

install_target_runtime_dlls(TARGETS WindowsAppSdk::Bootstrap
    DESTINATION ${CMAKE_BINARY_DIR}
)