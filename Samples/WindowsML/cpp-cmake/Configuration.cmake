#----------------------------------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------------------------------
add_library(Configuration INTERFACE)

target_compile_definitions(Configuration
    INTERFACE
        UNICODE
        _UNICODE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
)

target_compile_features(Configuration
    INTERFACE
        cxx_std_23
)

target_compile_options(Configuration
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/d1nodatetime>
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
        $<$<CXX_COMPILER_ID:MSVC>:/W3>
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
        $<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:Debug>>:/GL>>
        $<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:Debug>>:/O2>>
)

target_link_options(Configuration
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/MANIFEST:NO>
        $<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:Debug>>:/LTCG>>
        $<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>>
)
