include_guard(GLOBAL)

function(cppwinrt_generate_headers)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "" "WINMDS;WINMD_GLOBS")

    set(cppwinrt_args "")
    string(APPEND cppwinrt_args "-ref sdk+\n")
    string(APPEND cppwinrt_args "-ref \"${CURRENT_INSTALLED_DIR}/lib\"\n")
    string(APPEND cppwinrt_args "-output \"${CURRENT_PACKAGES_DIR}/include\"\n")
    string(APPEND cppwinrt_args "-verbose\n")
    
    set(winmds "")
    if(DEFINED arg_WINMD_GLOBS)
        foreach (glob IN LISTS arg_WINMD_GLOBS)
            message(STATUS "cppwinrt_generate_headers: Adding glob: ${glob}")
            file(GLOB glob_results LIST_DIRECTORIES false "${glob}")
            list(APPEND winmds "${glob_results}")
        endforeach()
    else()
        set(winmds "${arg_WINMDS}")
    endif()

    message(STATUS "cppwinrt_generate_headers: winmds: ${winmds}")
    
    foreach (winmd IN LISTS winmds)
        get_filename_component(_winmd_dest "${winmd}" NAME)
        set(_winmd_dest "${CURRENT_PACKAGES_DIR}/lib/${_winmd_dest}")
        file(COPY ${winmd} DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

        string(APPEND cppwinrt_args "-input \"${_winmd_dest}\"\n")
        message(STATUS "cppwinrt_generate_headers: Adding winmd: ${_winmd_dest}")
    endforeach()
    
    set(cppwinrt_rsp "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}.cppwinrt.rsp")
    file(WRITE "${cppwinrt_rsp}" "${cppwinrt_args}")

    vcpkg_execute_required_process(
        COMMAND "${CURRENT_INSTALLED_DIR}/tools/cppwinrt/cppwinrt.exe" "@${cppwinrt_rsp}"
        WORKING_DIRECTORY "${CURRENT_BUILDTREES_DIR}"
        LOGNAME "${TARGET_TRIPLET}.cppwinrt.log"
    )
endfunction()