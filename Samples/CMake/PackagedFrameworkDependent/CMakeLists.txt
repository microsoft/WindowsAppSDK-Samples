#----------------------------------------------------------------------------------------------------------------------
# PackagedFrameworkDependent - Framework-dependent deployment with MSIX packaging (new model)
#----------------------------------------------------------------------------------------------------------------------
project(PackagedFrameworkDependent LANGUAGES CXX)

add_executable(PackagedFrameworkDependent WIN32
    main.cpp
)

# PackageType defaults to "MSIX" — DeploymentManager auto-init is enabled by default
set_target_properties(PackagedFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # DeploymentManager auto-init is on by default for packaged framework-dependent apps (WindowsPackageType=MSIX).
    # Setting it explicitly here as an example of property usage, but it could be omitted since TRUE is the default.
    WindowsAppSdkDeploymentManagerInitialize TRUE
    # WindowsAppSDKSelfContained unset (default FALSE) → every bare WindowsAppSDK.<Component>
    # target resolves to its _Framework variant.
)

target_link_libraries(PackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite
        Microsoft.WindowsAppSDK.Widgets
        Microsoft.WindowsAppSDK.AI
        Microsoft.WindowsAppSDK.ML
        Microsoft.WindowsAppSDK.WinUI
)

wasdk_post_build_copy(PackagedFrameworkDependent)

# Generate AppxManifest.xml with framework PackageDependency auto-injected
wasdk_generate_appx_manifest(
    TARGET PackagedFrameworkDependent
    IDENTITY_NAME "CMake-PackagedFrameworkDependent"
    IDENTITY_PUBLISHER "CN=TestPublisher"
    IDENTITY_VERSION "1.0.0.0"
    DISPLAY_NAME "PackagedFrameworkDependent (CMake)"
    DESCRIPTION "Packaged framework-dependent CMake test"
)

# Copy Logo to the output directory for MSIX registration
add_custom_command(TARGET PackagedFrameworkDependent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:PackagedFrameworkDependent>
)
