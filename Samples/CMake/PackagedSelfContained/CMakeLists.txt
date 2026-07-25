#----------------------------------------------------------------------------------------------------------------------
# PackagedSelfContained - Self-contained deployment with MSIX packaging (new model)
#----------------------------------------------------------------------------------------------------------------------
project(PackagedSelfContained LANGUAGES CXX)

add_executable(PackagedSelfContained WIN32
    main.cpp
)

set_target_properties(PackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    # One property — every bare WindowsAppSDK.<Component> target resolves to its _SelfContained variant.
    WindowsAppSDKSelfContained TRUE
)

target_link_libraries(PackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite
        Microsoft.WindowsAppSDK.Widgets
        Microsoft.WindowsAppSDK.AI
        Microsoft.WindowsAppSDK.ML
        Microsoft.WindowsAppSDK.WinUI
)

# Copy AppxManifest.xml and Logo to the output directory for MSIX registration
add_custom_command(TARGET PackagedSelfContained POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/AppxManifest.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:PackagedSelfContained>
)
