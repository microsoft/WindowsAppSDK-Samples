cmake_minimum_required(VERSION 3.10)

project(cmake-ai-generator)

find_package(wil CONFIG REQUIRED)
find_package(cppwinrt CONFIG REQUIRED)
find_package(windowsappsdk CONFIG REQUIRED)

add_executable(cmake-ai-generator
    main.cpp
)

target_link_libraries(cmake-ai-generator
    PRIVATE
        Microsoft::CppWinRT
        WIL::WIL
        WindowsAppSdk::WindowsAppSdk
        RuntimeObject # For cppwinrt's use of winrt::hresult_error
)

# Run CPPWINRT_TOOL over a list of .winmd files

set(CPPWINRT_IN_WINMDS "")
list(APPEND CPPWINRT_IN_WINMDS ${WINAPPSDK_WINMDS})

set(cppwinrt_args "")
foreach(winmd_file ${CPPWINRT_IN_WINMDS})
    string(APPEND cppwinrt_args "-in \"${winmd_file}\"\n")
endforeach()
string(APPEND cppwinrt_args "-overwrite\n")
string(APPEND cppwinrt_args "-optimize\n")
string(APPEND cppwinrt_args "-ref sdk+\n")
string(APPEND cppwinrt_args "-out \"${CMAKE_BINARY_DIR}/generated\"\n")

# Write the arguments to a response file
file(WRITE "${CMAKE_BINARY_DIR}/cppwinrt.rsp" "${cppwinrt_args}")
set(touch_file "${CMAKE_BINARY_DIR}/cppwinrt.touch")

add_custom_command(
    OUTPUT ${touch_file}
    COMMAND ${CPPWINRT_TOOL} "@${CMAKE_BINARY_DIR}/cppwinrt.rsp"
    COMMAND "${CMAKE_COMMAND}" -E touch "${touch_file}"
    DEPENDS ${CPPWINRT_IN_WINMDS}
    COMMENT "Generating C++/WinRT code from WinMD files"
    )

add_custom_target(
    GenerateCppWinRT
    DEPENDS "${touch_file}"
    )

add_dependencies(cmake-ai-generator GenerateCppWinRT)