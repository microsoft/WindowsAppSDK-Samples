#----------------------------------------------------------------------------------------------------------------------
# Metapackage_PackagedFrameworkDependent
# Packaged + Framework-dependent. Default mode (no WindowsAppSDKSelfContained property set).
#----------------------------------------------------------------------------------------------------------------------
project(Metapackage_PackagedFrameworkDependent LANGUAGES CXX)

add_executable(Metapackage_PackagedFrameworkDependent WIN32
    main.cpp
)

set_target_properties(Metapackage_PackagedFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # Packaged FW: WindowsPackageType defaults to MSIX → DeploymentManager auto-init enabled by default.
    WindowsAppSdkDeploymentManagerInitialize TRUE
    # WindowsAppSDKSelfContained unset (default FALSE) → basic component targets resolve to _Framework.
)

# Single link entry — basic component targets inside the umbrella resolve to _Framework.
target_link_libraries(Metapackage_PackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK
)

wasdk_post_build_copy(Metapackage_PackagedFrameworkDependent)

# Generate AppxManifest.xml with framework PackageDependency auto-injected.
wasdk_generate_appx_manifest(
    TARGET Metapackage_PackagedFrameworkDependent
    IDENTITY_NAME "CMake-Metapackage-PackagedFrameworkDependent"
    IDENTITY_PUBLISHER "CN=TestPublisher"
    IDENTITY_VERSION "1.0.0.0"
    DISPLAY_NAME "Metapackage Packaged FW (CMake)"
    DESCRIPTION "Packaged framework-dependent CMake test using metapackage umbrella"
)

add_custom_command(TARGET Metapackage_PackagedFrameworkDependent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:Metapackage_PackagedFrameworkDependent>
)
