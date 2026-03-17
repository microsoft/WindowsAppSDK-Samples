#----------------------------------------------------------------------------------------------------------------------
# Common configuration for all baseline test apps
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
        cxx_std_20
)

target_compile_options(Configuration
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
        $<$<CXX_COMPILER_ID:MSVC>:/W3>
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
)

target_link_options(Configuration
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:Debug>>:/LTCG>>
        $<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>>
)
