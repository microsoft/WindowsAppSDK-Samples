get_filename_component(_packages_dir "${CMAKE_CURRENT_LIST_DIR}" PATH)
get_filename_component(_packages_dir  "${_packages_dir}" PATH)

if(NOT TARGET WindowsAppSdk::DWriteCore)
   add_library(WindowsAppSdk::DWriteCore STATIC IMPORTED)
   set_property(
      TARGET WindowsAppSdk::DWriteCore 
      PROPERTY IMPORTED_LOCATION "${_packages_dir}/lib/DWriteCore.lib"
    )
endif()


if(NOT TARGET WindowsAppSdk::Bootstrap)
   add_library(WindowsAppSdk::Bootstrap SHARED IMPORTED)
   set_target_properties(WindowsAppSdk::Bootstrap 
      PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_packages_dir}/include"
        IMPORTED_IMPLIB "${_packages_dir}/lib/Microsoft.WindowsAppRuntime.Bootstrap.lib"
        IMPORTED_LOCATION "${_packages_dir}/bin/Microsoft.WindowsAppRuntime.Bootstrap.dll"
        RUNTIME_DLLS "${_packages_dir}/bin/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )
endif()

if(NOT TARGET WindowsAppSdk::Runtime)
   add_library(WindowsAppSdk::Runtime STATIC IMPORTED)
   set_property(
      TARGET WindowsAppSdk::Runtime 
      PROPERTY IMPORTED_LOCATION "${_packages_dir}/lib/Microsoft.WindowsAppRuntime.lib"
    )
endif()

if(NOT TARGET Microsoft::WindowsAppSdk)
   add_library(Microsoft::WindowsAppSdk INTERFACE IMPORTED)
 #  add_custom_target( DEPENDS "${wasdk_stamp}")
   target_link_libraries(Microsoft::WindowsAppSdk INTERFACE
      WindowsAppSdk::DWriteCore
      WindowsAppSdk::Bootstrap
      WindowsAppSdk::Runtime
   )
endif()

function(add_winappsdk)
    set(target_name "${ARGV0}")
    set(wasdk_inc_path "${CMAKE_CURRENT_BINARY_DIR}/wasdk/inc")
    set(wasdk_stamp "${wasdk_inc_path}/.done")

    find_package(unofficial-webview2 CONFIG REQUIRED)
    find_library(WEBVIEW2_WINMD "Microsoft.Web.WebView2.Core.winmd")

    set(cppwinrt_args "")
    string(APPEND cppwinrt_args "-ref sdk+\n")
    string(APPEND cppwinrt_args "-ref \"${WEBVIEW2_WINMD}\"\n")
    string(APPEND cppwinrt_args "-optimize\n")
    string(APPEND cppwinrt_args "-output \"${wasdk_inc_path}\"\n")
    string(APPEND cppwinrt_args "-verbose\n")


    file(GLOB wasdk_winmds LIST_DIRECTORIES false "${WINAPPSDK_ROOT}/winmd/**/*.winmd")

    foreach (winmd IN LISTS wasdk_winmds)
        string(APPEND cppwinrt_args "-input \"${winmd}\"\n")
        message("wasdk_projection: Adding winmd: ${winmd}")
    endforeach()
    
    set(cppwinrt_rsp "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}.cppwinrt.rsp")
    file(WRITE "${cppwinrt_rsp}" "${cppwinrt_args}")

    add_custom_command(
        TARGET ${target_name}
        PRE_BUILD
        DEPENDS ${wasdk_winmds} ${webview2_winmds}
        COMMAND "${CMAKE_COMMAND}" -E echo "Generating WinAppSDK projection for ${target_name}"
        COMMAND "${CPPWINRT_TOOL}" "@${cppwinrt_rsp}"
        COMMAND "${CMAKE_COMMAND}" -E touch "${wasdk_stamp}")

#    add_custom_target(${target_name} DEPENDS "${wasdk_stamp}")
endfunction()

unset(_packages_dir)
