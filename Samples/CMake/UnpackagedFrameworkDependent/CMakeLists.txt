#----------------------------------------------------------------------------------------------------------------------
# UnpackagedFrameworkDependent - Framework-dependent deployment, no MSIX packaging (new model)
#----------------------------------------------------------------------------------------------------------------------
project(UnpackagedFrameworkDependent LANGUAGES CXX)

add_executable(UnpackagedFrameworkDependent WIN32
    main.cpp
)

set_target_properties(UnpackagedFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # Unpackaged app: set PackageType to None so bootstrap auto-init is enabled by default
    WindowsPackageType "None"
    # Bootstrap auto-init is on by default for unpackaged framework-dependent apps (WindowsPackageType=None).
    # Setting it explicitly here as an example of property usage, it can be omitted otherwise.
    WindowsAppSdkBootstrapInitialize TRUE
    # WindowsAppSDKSelfContained unset (default FALSE) → every bare WindowsAppSDK.<Component>
    # target resolves to its _Framework variant.
)

target_link_libraries(UnpackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite
        Microsoft.WindowsAppSDK.Widgets
        Microsoft.WindowsAppSDK.AI
        Microsoft.WindowsAppSDK.ML
        Microsoft.WindowsAppSDK.WinUI
)

wasdk_post_build_copy(UnpackagedFrameworkDependent)
