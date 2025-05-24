vcpkg_find_acquire_program(NUGET)

set(ENV{NUGET_PACKAGES} "${BUILDTREES_DIR}/nuget")
set(PACKAGE_NAME "Microsoft.WindowsAppSDK")
set(PACKAGE_VERSION ${VERSION})

vcpkg_execute_required_process(
    ALLOW_IN_DOWNLOAD_MODE
    COMMAND ${NUGET} install ${PACKAGE_NAME} -version ${PACKAGE_VERSION} -NonInteractive
        -OutputDirectory "${CURRENT_BUILDTREES_DIR}"
    WORKING_DIRECTORY "${CURRENT_BUILDTREES_DIR}"
    LOGNAME nuget-${TARGET_TRIPLET}
    )

# Get all the WinMDs in the "lib" directory, but only the latest SDK versions
file(INSTALL
    "${CURRENT_BUILDTREES_DIR}/include"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.idl" 
        PATTERN "*.cpp")

file(INSTALL
    "${CURRENT_BUILDTREES_DIR}/lib/uap10.0"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib"
    FILES_MATCHING PATTERN "*.winmd")

file(INSTALL
    "${CURRENT_BUILDTREES_DIR}/lib/uap10.0.18362"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib"
    FILES_MATCHING PATTERN "*.winmd")

file(GLOB
    WINAPPSDK_BOOTSTRAP_DLL
    LIST_DIRECTORIES false 
    "${CURRENT_BUILDTREES_DIR}/**/runtimes/win-${VCPKG_TARGET_ARCHITECTURE}/native/Microsoft.WindowsAppRuntime.Bootstrap.dll")

file(INSTALL
    ${WINAPPSDK_BOOTSTRAP_DLL}
    DESTINATION "${CURRENT_PACKAGES_DIR}/bin")

file(INSTALL 
    "${CMAKE_CURRENT_LIST_DIR}/windowsappsdk-config.cmake"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(GLOB
    WINAPPSDK_IMPORT_LIBS
    LIST_DIRECTORIES false
    "${CURRENT_BUILDTREES_DIR}/**/lib/win-${VCPKG_TARGET_ARCHITECTURE}/*.lib"
    "${CURRENT_BUILDTREES_DIR}/**/lib/win10-${VCPKG_TARGET_ARCHITECTURE}/*.lib"
    )

file(INSTALL ${WINAPPSDK_IMPORT_LIBS}
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

#--- Copy license
configure_file("${CURRENT_BUILDTREES_DIR}/${PACKAGE_NAME}.${VERSION}/license.txt" "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" COPYONLY)

#--- Generate the projections

set(WINAPPSDK_INCLUDES "${CURRENT_PACKAGES_DIR}/include")
set(cppwinrt_rsp "${WINAPPSDK_INCLUDES}/.cppwinrt.rsp")
set(WEBVIEW2_WINMD "${CURRENT_HOST_INSTALLED_DIR}/lib/Microsoft.Web.WebView2.Core.winmd")

file(GLOB WINAPPSDK_WINMDS LIST_DIRECTORIES false "${CURRENT_BUILDTREES_DIR}/lib/uap10.0/*.winmd")
file(GLOB _WINAPPSDK_WINMDS_UAP10_LATEST LIST_DIRECTORIES false "${CURRENT_BUILDTREES_DIR}/lib/uap10.0.18362/*.winmd")
list(APPEND WINAPPSDK_WINMDS ${_WINAPPSDK_WINMDS_UAP10_LATEST})

set(cppwinrt_args "")
string(APPEND cppwinrt_args "-ref sdk+\n")
string(APPEND cppwinrt_args "-ref \"${WEBVIEW2_WINMD}\"\n")
string(APPEND cppwinrt_args "-optimize\n")
string(APPEND cppwinrt_args "-output \"${WINAPPSDK_INCLUDES}\"\n")
string(APPEND cppwinrt_args "-verbose\n")
string(APPEND cppwinrt_args "-exclude Windows\n")
foreach (winmd IN LISTS WINAPPSDK_WINMDS)
    string(APPEND cppwinrt_args "-input \"${winmd}\"\n")
endforeach()

file(WRITE "${cppwinrt_rsp}" "${cppwinrt_args}")

find_program(CPPWINRT_TOOL cppwinrt PATHS "${CURRENT_HOST_INSTALLED_DIR}/tools/cppwinrt" NO_DEFAULT_PATH REQUIRED)

vcpkg_execute_required_process(
    COMMAND "${CPPWINRT_TOOL}" "@${cppwinrt_rsp}"
    WORKING_DIRECTORY "${CURRENT_PACKAGES_DIR}"
    LOGNAME "wasdk-project-${TARGET_TRIPLET}")

include_guard(GLOBAL)
