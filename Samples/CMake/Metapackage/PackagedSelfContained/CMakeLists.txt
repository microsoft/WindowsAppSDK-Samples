#----------------------------------------------------------------------------------------------------------------------
# Metapackage_PackagedSelfContained
# Packaged + Self-contained. WindowsAppSDKSelfContained=TRUE.
#----------------------------------------------------------------------------------------------------------------------
project(Metapackage_PackagedSelfContained LANGUAGES CXX)

add_executable(Metapackage_PackagedSelfContained WIN32
    main.cpp
)

set_target_properties(Metapackage_PackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # One property — every basic component target resolves to _SelfContained.
    WindowsAppSDKSelfContained TRUE
)

# Single link entry — bare component targets inside the umbrella resolve to _SelfContained.
target_link_libraries(Metapackage_PackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK
)

wasdk_post_build_copy(Metapackage_PackagedSelfContained)

# Generate AppxManifest.xml. Self-contained apps still benefit from a packaged manifest for MSIX
# distribution. The helper can be used here as well or you can provide your own manifest if you need to customize it beyond what the helper supports for the package self contained scenario.
wasdk_generate_appx_manifest(
    TARGET Metapackage_PackagedSelfContained
    IDENTITY_NAME "CMake-Metapackage-PackagedSelfContained"
    IDENTITY_PUBLISHER "CN=TestPublisher"
    IDENTITY_VERSION "1.0.0.0"
    DISPLAY_NAME "Metapackage Packaged SC (CMake)"
    DESCRIPTION "Packaged self-contained CMake test using metapackage umbrella"
)

add_custom_command(TARGET Metapackage_PackagedSelfContained POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:Metapackage_PackagedSelfContained>
)
