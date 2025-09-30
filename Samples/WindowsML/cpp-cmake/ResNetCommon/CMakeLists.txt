#----------------------------------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------------------------------
project(ResNetCommon LANGUAGES CXX)

add_library(ResNetCommon STATIC
    ResNetModelHelper.cpp
)

target_precompile_headers(ResNetCommon
    PRIVATE
        pch.h
)

target_include_directories(ResNetCommon
    PUBLIC
        ./include
)

target_link_libraries(ResNetCommon
    PRIVATE
        Microsoft.Windows.CppWinRT
        Microsoft.Windows.ImplementationLibrary
)
