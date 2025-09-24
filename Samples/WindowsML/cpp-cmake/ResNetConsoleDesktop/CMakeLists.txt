#----------------------------------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------------------------------
project(ResNetConsoleDesktop LANGUAGES CXX)

add_executable(ResNetConsoleDesktop
    app.manifest
    main.cpp
)

target_precompile_headers(ResNetConsoleDesktop
    PRIVATE
        pch.h
)

target_link_libraries(ResNetConsoleDesktop
    PRIVATE
        ResNetCommon
        Microsoft.WindowsAppSDK.ML_Framework # Use 'framework' mode.
        Microsoft.Windows.ImplementationLibrary
)

post_build_runtime_dll_copy(ResNetConsoleDesktop)

add_custom_command(TARGET ResNetConsoleDesktop POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:ResNetConsoleDesktop> ${RESNET_MODEL_FILES} ${RESOURCE_FILES}
)
