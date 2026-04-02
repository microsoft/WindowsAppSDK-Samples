vcpkg_find_acquire_program(NUGET)

set(ENV{NUGET_PACKAGES} "${BUILDTREES_DIR}/nuget")

vcpkg_execute_required_process(
    ALLOW_IN_DOWNLOAD_MODE
    COMMAND ${NUGET} install "Microsoft.WindowsAppSDK" -version ${VERSION} -NonInteractive
        -OutputDirectory "${CURRENT_BUILDTREES_DIR}"
    WORKING_DIRECTORY "${CURRENT_BUILDTREES_DIR}"
    LOGNAME nuget-${TARGET_TRIPLET})

# Collect all component include files
file(GLOB
    winappsdk_includes
    LIST_DIRECTORIES false
    "${CURRENT_BUILDTREES_DIR}/**/include/*.*")
file(INSTALL ${winappsdk_includes}
    DESTINATION "${CURRENT_PACKAGES_DIR}/include")

# Collect all component metadata files
file(GLOB
    winappsdk_winmds
    LIST_DIRECTORIES false
    "${CURRENT_BUILDTREES_DIR}/**/metadata/*.winmd"
    "${CURRENT_BUILDTREES_DIR}/**/metadata/10.0.18362.0/*.winmd"    # include non-standard paths
    "${CURRENT_BUILDTREES_DIR}/**/lib/*.winmd")                     # include WebView2 metadata 

# Collect all component libraries
file(GLOB
    winappsdk_import_libs
    LIST_DIRECTORIES false
    "${CURRENT_BUILDTREES_DIR}/**/lib/win-${VCPKG_TARGET_ARCHITECTURE}/*.lib"
    "${CURRENT_BUILDTREES_DIR}/**/lib/win10-${VCPKG_TARGET_ARCHITECTURE}/*.lib"             # include non-standard paths
    "${CURRENT_BUILDTREES_DIR}/**/lib/native/win10-${VCPKG_TARGET_ARCHITECTURE}/*.lib")     # include non-standard paths
file(INSTALL ${winappsdk_import_libs}
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

# Collect all component runtime files
file(GLOB
    winappsdk_runtime_files
    LIST_DIRECTORIES false 
    "${CURRENT_BUILDTREES_DIR}/**/runtimes/win-${VCPKG_TARGET_ARCHITECTURE}/native/*.*")
file(INSTALL ${winappsdk_runtime_files}
    DESTINATION "${CURRENT_PACKAGES_DIR}/bin")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/windowsappsdk-config.cmake"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" 
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

#--- Copy license
configure_file("${CURRENT_BUILDTREES_DIR}/Microsoft.WindowsAppSDK.${VERSION}/license.txt" "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" COPYONLY)

#--- Generate C++/WinRT projection headers
set(winappsdk_includes "${CURRENT_PACKAGES_DIR}/include")
set(cppwinrt_rsp "${winappsdk_includes}/.cppwinrt.rsp")

set(cppwinrt_args "")
string(APPEND cppwinrt_args "-input sdk+\n")
foreach(winmd ${winappsdk_winmds})
    string(APPEND cppwinrt_args "-input \"${winmd}\"\n")
endforeach()
string(APPEND cppwinrt_args "-optimize\n")
string(APPEND cppwinrt_args "-output \"${winappsdk_includes}\"\n")
string(APPEND cppwinrt_args "-verbose\n")

file(WRITE "${cppwinrt_rsp}" "${cppwinrt_args}")

vcpkg_execute_required_process(
    ALLOW_IN_DOWNLOAD_MODE
    COMMAND ${NUGET} install "Microsoft.Windows.CppWinRT" -NonInteractive
        -OutputDirectory "${CURRENT_BUILDTREES_DIR}"
    WORKING_DIRECTORY "${CURRENT_BUILDTREES_DIR}"
    LOGNAME nuget-${TARGET_TRIPLET})

file(GLOB
    cppwinrt_tool
    LIST_DIRECTORIES false 
    "${CURRENT_BUILDTREES_DIR}/Microsoft.Windows.CppWinRT*/bin/cppwinrt.exe")

vcpkg_execute_required_process(
    COMMAND "${cppwinrt_tool}" "@${cppwinrt_rsp}"
    WORKING_DIRECTORY "${CURRENT_PACKAGES_DIR}"
    LOGNAME "wasdk-project-${TARGET_TRIPLET}")

include_guard(GLOBAL)
