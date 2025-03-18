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

if(NOT TARGET WindowsAppSdk::WindowsAppSdk)
   add_library(WindowsAppSdk::WindowsAppSdk INTERFACE IMPORTED)
   target_link_libraries(WindowsAppSdk::WindowsAppSdk INTERFACE
      WindowsAppSdk::DWriteCore
      WindowsAppSdk::Bootstrap
      WindowsAppSdk::Runtime
   )
endif()

# Outputs from this package

file(GLOB_RECURSE WINAPPSDK_WINMDS 
    "${_packages_dir}/lib/uap10.0/*.winmd"
    "${_packages_dir}/lib/uap10.0.18362/*.winmd"
    )

unset(_packages_dir)
