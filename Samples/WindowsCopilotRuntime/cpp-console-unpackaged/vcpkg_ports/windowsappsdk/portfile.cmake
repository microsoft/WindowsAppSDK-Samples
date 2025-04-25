
vcpkg_download_distfile(ARCHIVE
    URLS "https://www.nuget.org/api/v2/package/Microsoft.WindowsAppSDK/${VERSION}"
    FILENAME "Microsoft.WindowsAppSDK.${VERSION}.zip"
    SHA512 00a4500de8e9f449a55fb73a6fe4305ae5e4edc3ac27618a76420567870e76e14da57d07b6953fd7c6f127d3ea42ac9f3aa26a20b347e6ee7b753ca61bb1bba8
)


vcpkg_extract_source_archive(
    PACKAGE_PATH
    ARCHIVE "${ARCHIVE}"
    NO_REMOVE_ONE_LEVEL
)

file(REMOVE_RECURSE "${PACKAGE_PATH}/lib/uap10.0/Microsoft.UI/Themes")
file(REMOVE_RECURSE "${PACKAGE_PATH}/lib/uap10.0/Microsoft.UI")

# Get all the WinMDs in the "lib" directory, but only the latest SDK versions
file(INSTALL
    "${PACKAGE_PATH}/include"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.idl" 
        PATTERN "*.cpp")

file(INSTALL
    "${PACKAGE_PATH}/lib/uap10.0"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib"
    FILES_MATCHING PATTERN "*.winmd")

file(INSTALL
    "${PACKAGE_PATH}/lib/uap10.0.18362"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib"
    FILES_MATCHING PATTERN "*.winmd")

file(INSTALL
    "${PACKAGE_PATH}/runtimes/win-${VCPKG_TARGET_ARCHITECTURE}/native/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    DESTINATION "${CURRENT_PACKAGES_DIR}/bin")

file(INSTALL 
    "${CMAKE_CURRENT_LIST_DIR}/windowsappsdk-config.cmake"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(INSTALL "${PACKAGE_PATH}/lib/win10-${VCPKG_TARGET_ARCHITECTURE}/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

#--- Copy license
configure_file("${PACKAGE_PATH}/license.txt" "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" COPYONLY)

#--- Generate the projections

set(WINAPPSDK_INCLUDES "${CURRENT_PACKAGES_DIR}/include")
set(cppwinrt_rsp "${WINAPPSDK_INCLUDES}/.cppwinrt.rsp")
set(WEBVIEW2_WINMD "${CURRENT_HOST_INSTALLED_DIR}/lib/Microsoft.Web.WebView2.Core.winmd")

file(GLOB WINAPPSDK_WINMDS LIST_DIRECTORIES false "${PACKAGE_PATH}/lib/uap10.0/*.winmd")
file(GLOB _WINAPPSDK_WINMDS_UAP10_LATEST LIST_DIRECTORIES false "${PACKAGE_PATH}/lib/uap10.0.18362/*.winmd")
list(APPEND WINAPPSDK_WINMDS ${_WINAPPSDK_WINMDS_UAP10_LATEST})

set(cppwinrt_args "")
string(APPEND cppwinrt_args "-ref sdk+\n")
string(APPEND cppwinrt_args "-ref \"${WEBVIEW2_WINMD}\"\n")
string(APPEND cppwinrt_args "-optimize\n")
string(APPEND cppwinrt_args "-output \"${WINAPPSDK_INCLUDES}\"\n")
string(APPEND cppwinrt_args "-verbose\n")
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
