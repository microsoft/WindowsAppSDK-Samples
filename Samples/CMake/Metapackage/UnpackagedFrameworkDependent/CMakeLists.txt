#----------------------------------------------------------------------------------------------------------------------
# Metapackage_UnpackagedFrameworkDependent
# Unpackaged + Framework-dependent. WindowsPackageType=None → Bootstrap auto-init enabled by default.
#----------------------------------------------------------------------------------------------------------------------
project(Metapackage_UnpackagedFrameworkDependent LANGUAGES CXX)

add_executable(Metapackage_UnpackagedFrameworkDependent WIN32
    main.cpp
)

set_target_properties(Metapackage_UnpackagedFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # Unpackaged: no MSIX. Bootstrap auto-init is enabled by default when WindowsPackageType=None.
    WindowsPackageType "None"
    WindowsAppSdkBootstrapInitialize TRUE
    # WindowsAppSDKSelfContained unset (default FALSE) → basic component targets resolve to _Framework.
)

# Single link entry — basic component targets inside the umbrella resolve to _Framework.
target_link_libraries(Metapackage_UnpackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK
)

wasdk_post_build_copy(Metapackage_UnpackagedFrameworkDependent)

# No wasdk_generate_appx_manifest() — this is an unpackaged exe.
