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
   add_library(WindowsAppSdk::Bootstrap STATIC IMPORTED)
   set_property(
      TARGET WindowsAppSdk::Bootstrap 
      PROPERTY IMPORTED_LOCATION "${_packages_dir}/lib/Microsoft.WindowsAppRuntime.Bootstrap.lib"
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


unset(_packages_dir)
