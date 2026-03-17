#----------------------------------------------------------------------------------------------------------------------
# PackagedFrameworkDependent - Framework-dependent deployment with MSIX packaging
#
# After building, register the MSIX layout and launch via AUMID:
#   Add-AppxPackage -Register <build-dir>\PackagedFrameworkDependent\AppxManifest.xml
#   Start-Process "shell:AppsFolder\<PFN>!App"
#----------------------------------------------------------------------------------------------------------------------
project(PackagedFrameworkDependent LANGUAGES CXX)

add_executable(PackagedFrameworkDependent WIN32
    main.cpp
)

# Framework-dependent: use dynamic CRT
set_target_properties(PackagedFrameworkDependent PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

# Use Foundation_Framework but without bootstrapper (MSIX provides framework package via package graph)
target_link_libraries(PackagedFrameworkDependent
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_Framework
        Microsoft.WindowsAppSDK.DWrite
)

post_build_runtime_dll_copy(PackagedFrameworkDependent)

# Copy AppxManifest.xml and Logo to the output directory for MSIX registration
add_custom_command(TARGET PackagedFrameworkDependent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/AppxManifest.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/Logo.png
        $<TARGET_FILE_DIR:PackagedFrameworkDependent>
)
