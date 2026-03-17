#----------------------------------------------------------------------------------------------------------------------
# UnpackagedSelfContained - Self-contained deployment, no MSIX packaging
#----------------------------------------------------------------------------------------------------------------------
project(UnpackagedSelfContained LANGUAGES CXX)

add_executable(UnpackagedSelfContained WIN32
    main.cpp
)

# Self-contained: use static VC runtime (Hybrid CRT)
set_target_properties(UnpackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_link_libraries(UnpackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_SelfContained
        Microsoft.WindowsAppSDK.InteractiveExperiences_SelfContained
        Microsoft.WindowsAppSDK.DWrite_SelfContained
)

post_build_runtime_dll_copy(UnpackagedSelfContained)
