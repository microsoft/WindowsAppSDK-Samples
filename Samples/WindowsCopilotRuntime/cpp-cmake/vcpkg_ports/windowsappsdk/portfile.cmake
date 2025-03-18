
vcpkg_download_distfile(ARCHIVE
    URLS "https://www.nuget.org/api/v2/package/Microsoft.WindowsAppSDK/${VERSION}"
    FILENAME "Microsoft.WindowsAppSDK.${VERSION}.zip"
    SHA512 dfc5f41ac1b48555bea277ca6f89ac7c4dd6766f5c58864876825c7eefd469b1c59484afa79ac84e2a281fc4fd983e611672dc8efc6b960a938a5b6bfa102f14
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

cppwinrt_generate_headers(
    WINMD_GLOBS
    "${PACKAGE_PATH}/lib/uap10.0/*.winmd"
    "${PACKAGE_PATH}/lib/uap10.0.18362/*.winmd")

file(COPY
    "${CMAKE_CURRENT_LIST_DIR}/windowsappsdk-config.cmake"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(COPY "${PACKAGE_PATH}/lib/win10-${VCPKG_TARGET_ARCHITECTURE}/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

#--- Copy license
configure_file("${PACKAGE_PATH}/license.txt" "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" COPYONLY)
