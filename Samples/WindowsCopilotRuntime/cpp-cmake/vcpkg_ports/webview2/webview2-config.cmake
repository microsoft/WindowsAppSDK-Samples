get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_DIR}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

if(NOT TARGET WebView2::WebView2)
    if(EXISTS "${_IMPORT_PREFIX}/lib/WebView2LoaderStatic.lib")
        add_library(WebView2::WebView2 STATIC IMPORTED)
        set_target_properties(WebView2::WebView2
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
                IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/WebView2LoaderStatic.lib")
    else()
        add_library(WebView2::WebView2 SHARED IMPORTED)
        set_target_properties(WebView2::WebView2
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
                IMPORTED_LOCATION "${_IMPORT_PREFIX}/bin/WebView2Loader.dll"
                IMPORTED_IMPLIB "${_IMPORT_PREFIX}/lib/WebView2Loader.dll.lib")
    endif()
endif()


file(GLOB_RECURSE WEBVIEW2_WINMDS 
    "${_IMPORT_PREFIX}/lib/Microsoft.Web.WebView2.Core.winmd"
    )

unset(_IMPORT_PREFIX)
