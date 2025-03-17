cmake_minimum_required(VERSION 3.10)

project(cmake-ai-generator)

find_package(wil CONFIG REQUIRED)
find_package(cppwinrt CONFIG REQUIRED)

add_executable(cmake-ai-generator
    main.cpp
)

target_link_libraries(cmake-ai-generator
    PRIVATE
        wil::wil
        cppwinrt::cppwinrt
)