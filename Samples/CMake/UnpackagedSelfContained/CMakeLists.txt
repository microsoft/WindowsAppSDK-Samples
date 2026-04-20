#----------------------------------------------------------------------------------------------------------------------
# UnpackagedSelfContained - Self-contained deployment, no MSIX packaging
#----------------------------------------------------------------------------------------------------------------------
project(UnpackagedSelfContained LANGUAGES CXX)

add_executable(UnpackagedSelfContained WIN32
    main.cpp
)

set_target_properties(UnpackagedSelfContained PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

target_link_libraries(UnpackagedSelfContained
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_SelfContained
        Microsoft.WindowsAppSDK.InteractiveExperiences_SelfContained
        Microsoft.WindowsAppSDK.DWrite_SelfContained
        Microsoft.WindowsAppSDK.Widgets_SelfContained
        Microsoft.WindowsAppSDK.AI_SelfContained
        Microsoft.WindowsAppSDK.ML_SelfContained
        Microsoft.WindowsAppSDK.WinUI_SelfContained
)

post_build_runtime_dll_copy(UnpackagedSelfContained)
post_build_selfcontained_dll_copy(UnpackagedSelfContained)
