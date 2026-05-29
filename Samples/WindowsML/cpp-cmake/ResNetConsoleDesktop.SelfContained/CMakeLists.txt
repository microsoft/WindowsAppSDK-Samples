#----------------------------------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------------------------------
project(ResNetConsoleDesktop.SelfContained LANGUAGES CXX)

add_executable(ResNetConsoleDesktop.SelfContained
    app.manifest
    main.cpp
)

target_precompile_headers(ResNetConsoleDesktop.SelfContained
    PRIVATE
        pch.h
)

target_link_libraries(ResNetConsoleDesktop.SelfContained
    PRIVATE
        ResNetCommon
        Microsoft.WindowsAppSDK.ML_SelfContained # Use 'self-contained' mode.
        Microsoft.Windows.ImplementationLibrary
)

post_build_runtime_dll_copy(ResNetConsoleDesktop.SelfContained)

add_custom_command(TARGET ResNetConsoleDesktop.SelfContained POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:ResNetConsoleDesktop.SelfContained> ${RESNET_MODEL_FILES} ${RESOURCE_FILES}
)
