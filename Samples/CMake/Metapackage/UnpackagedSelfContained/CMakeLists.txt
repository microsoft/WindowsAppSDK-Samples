#----------------------------------------------------------------------------------------------------------------------
# Metapackage_UnpackagedSelfContained
# Unpackaged + Self-contained. WindowsPackageType=None + WindowsAppSDKSelfContained=TRUE.
#----------------------------------------------------------------------------------------------------------------------
project(Metapackage_UnpackagedSelfContained LANGUAGES CXX)

add_executable(Metapackage_UnpackagedSelfContained WIN32
    main.cpp
)

set_target_properties(Metapackage_UnpackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    # Unpackaged: no MSIX. WindowsPackageType=None disables MSIX-only auto-inits.
    WindowsPackageType "None"
    # One property — every basic component target resolves to _SelfContained.
    WindowsAppSDKSelfContained TRUE
)

# Single link entry — bare component targets inside the umbrella resolve to _SelfContained.
target_link_libraries(Metapackage_UnpackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK
)

# No wasdk_prepare_appx_manifest() — this is an unpackaged exe.
