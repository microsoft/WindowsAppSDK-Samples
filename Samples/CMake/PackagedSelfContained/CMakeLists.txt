#----------------------------------------------------------------------------------------------------------------------
# PackagedSelfContained - Self-contained deployment with MSIX packaging
#
# After building, register the MSIX layout and launch via AUMID:
#   Add-AppxPackage -Register <build-dir>\PackagedSelfContained\AppxManifest.xml
#   Start-Process "shell:AppsFolder\<PFN>!App"
#----------------------------------------------------------------------------------------------------------------------
project(PackagedSelfContained LANGUAGES CXX)

add_executable(PackagedSelfContained WIN32
    main.cpp
)

# Self-contained: use static VC runtime (Hybrid CRT)
set_target_properties(PackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_link_libraries(PackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_SelfContained
        Microsoft.WindowsAppSDK.InteractiveExperiences_SelfContained
)

post_build_runtime_dll_copy(PackagedSelfContained)

# Copy AppxManifest.xml and Logo to the output directory for MSIX registration
add_custom_command(TARGET PackagedSelfContained POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/AppxManifest.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:PackagedSelfContained>
)
