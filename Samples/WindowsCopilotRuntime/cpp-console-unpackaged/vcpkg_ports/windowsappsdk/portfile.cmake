
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

set(WINAPPSDK_PATH "${PACKAGE_PATH}")

# Get all the WinMDs in the "lib" directory, but only the latest SDK versions
file(COPY
    "${PACKAGE_PATH}/include"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING PATTERN "*.h")

file(COPY
    "${PACKAGE_PATH}/include"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING PATTERN "*.idl")

file(COPY
    "${PACKAGE_PATH}/include"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING PATTERN "*.cpp")

file(COPY
    "${PACKAGE_PATH}/runtimes/win-${VCPKG_TARGET_ARCHITECTURE}/native/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    DESTINATION "${CURRENT_PACKAGES_DIR}/bin")

cppwinrt_generate_headers(
    WINMD_GLOBS
    "${PACKAGE_PATH}/lib/uap10.0/*.winmd"
    "${PACKAGE_PATH}/lib/uap10.0.18362/*.winmd")

file(COPY
    "${CMAKE_CURRENT_LIST_DIR}/windowsappsdk-config.cmake"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(COPY "${PACKAGE_PATH}/lib/win10-${VCPKG_TARGET_ARCHITECTURE}/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

#--- Copy license
configure_file("${PACKAGE_PATH}/license.txt" "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" COPYONLY)
