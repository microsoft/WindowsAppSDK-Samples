#----------------------------------------------------------------------------------------------------------------------
# UnpackagedFrameworkDependent - Framework-dependent deployment, no MSIX packaging
#----------------------------------------------------------------------------------------------------------------------
project(UnpackagedFrameworkDependent LANGUAGES CXX)

add_executable(UnpackagedFrameworkDependent WIN32
    main.cpp
)

# Framework-dependent: use dynamic CRT
set_target_properties(UnpackagedFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # Enable bootstrapper auto-init to load the framework package at startup
    WindowsAppSdkBootstrapInitialize TRUE
)

target_link_libraries(UnpackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_Framework
        Microsoft.WindowsAppSDK.DWrite
)

post_build_runtime_dll_copy(UnpackagedFrameworkDependent)
