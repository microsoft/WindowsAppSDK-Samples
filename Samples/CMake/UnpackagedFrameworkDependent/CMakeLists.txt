#----------------------------------------------------------------------------------------------------------------------
# UnpackagedFrameworkDependent - Framework-dependent deployment, no MSIX packaging
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
    # Setting it explicitly here as an example of property usage, it can be ommitted otherwise.
    WindowsAppSdkBootstrapInitialize TRUE
)

target_link_libraries(UnpackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_Framework
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite_Framework
        Microsoft.WindowsAppSDK.Widgets_Framework
        Microsoft.WindowsAppSDK.AI_Framework
        Microsoft.WindowsAppSDK.ML_Framework
        Microsoft.WindowsAppSDK.WinUI_Framework
)

post_build_runtime_dll_copy(UnpackagedFrameworkDependent)
