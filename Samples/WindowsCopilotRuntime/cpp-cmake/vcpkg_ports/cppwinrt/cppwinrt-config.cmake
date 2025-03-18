if(NOT TARGET Microsoft::CppWinRT)
  get_filename_component(_installed_root "${CMAKE_CURRENT_LIST_DIR}" PATH)
  get_filename_component(_installed_root "${_installed_root}" PATH)

  add_library(Microsoft::CppWinRT INTERFACE IMPORTED)
  set_target_properties(Microsoft::CppWinRT PROPERTIES
    INTERFACE_COMPILE_FEATURES cxx_std_20
    CPPWINRT_VS_PROPS_FILE "${CMAKE_CURRENT_LIST_DIR}/Microsoft.Windows.CppWinRT.props"
    CPPWINRT_VS_TARGETS_FILE "${CMAKE_CURRENT_LIST_DIR}/Microsoft.Windows.CppWinRT.targets"
  )
  target_link_libraries(
    Microsoft::CppWinRT 
    INTERFACE
      "${_installed_root}/lib/cppwinrt_fast_forwarder.lib"
      WindowsApp.lib
  )
  unset(_installed_root)
endif()

function(target_link_cppwinrt)
  set(target_name ${ARGV0})
  if ("${target_name}" STREQUAL "")
    message(FATAL_ERROR "Must provide a target name as the first argument")
  endif()

  set(link_type ${ARGV1})
  set(valid_link_types PUBLIC PRIVATE INTERFACE)
  list(FIND valid_link_types ${link_type} index)
  if (index EQUAL -1)
    message(FATAL_ERROR "Must provide valid linkage type (PUBLIC|PRIVATE|INTERFACE) as second argument")
  endif()

  target_link_libraries(${target_name} ${link_type} Microsoft::CppWinRT)

  get_target_property(CPPWINRT_PROPS Microsoft::CppWinRT CPPWINRT_VS_PROPS_FILE)
  set_target_properties(${target_name}
    PROPERTIES
      VS_PROJECT_IMPORT ${CPPWINRT_PROPS}
  )
endfunction()
