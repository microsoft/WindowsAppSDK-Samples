#----------------------------------------------------------------------------------------------------------------------
# UnpackagedSelfContained - Self-contained deployment, no MSIX packaging (new model)
#----------------------------------------------------------------------------------------------------------------------
project(UnpackagedSelfContained LANGUAGES CXX)

add_executable(UnpackagedSelfContained WIN32
    main.cpp
)

set_target_properties(UnpackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    # Unpackaged: no MSIX. Disables MSIX-only auto-inits and skips the
    # AppxManifest validation in wasdk_post_build_copy().
    WindowsPackageType "None"
    # One property — every bare WindowsAppSDK.<Component> target resolves to its _SelfContained variant.
    WindowsAppSDKSelfContained TRUE
)

target_link_libraries(UnpackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite
        Microsoft.WindowsAppSDK.Widgets
        Microsoft.WindowsAppSDK.AI
        Microsoft.WindowsAppSDK.ML
        Microsoft.WindowsAppSDK.WinUI
)

