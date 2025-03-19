cmake_minimum_required(VERSION 3.10)

project(cmake-ai-generator)

find_package(wil CONFIG REQUIRED)
find_package(cppwinrt CONFIG REQUIRED)
find_package(windowsappsdk CONFIG REQUIRED)
find_package(webview2 CONFIG REQUIRED)

add_executable(cmake-ai-generator
    main.cpp
    app.manifest
)

target_link_libraries(cmake-ai-generator
    INTERFACE
        WindowsAppSdk::Bootstrap
    PRIVATE
        Microsoft::CppWinRT
        WindowsAppSdk::WindowsAppSdk
        RuntimeObject # For cppwinrt's use of winrt::hresult_error
        OneCoreUAP
)

set(CMAKE_CXX_STANDARD 20)

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