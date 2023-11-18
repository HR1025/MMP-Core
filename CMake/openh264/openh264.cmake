# openh264 build wrapper

set(OPENH264_ARCH "")


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
        set(OPENH264_ARCH "x86_64")
    elseif(${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
        set(OPENH264_ARCH "arm64")
    elseif(${MMP_SYSTEM_DEVICE} MATCHES "MIPS")
        set(OPENH264_ARCH "arm")
    endif()
    ExternalProject_Add(openh264
        SOURCE_DIR ${MMP_PRO_TOP_DIR}/Extension/openh264
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make -C ${MMP_PRO_TOP_DIR}/Extension/openh264
            CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
            OS=linux ARCH=${OPENH264_ARCH} -j8
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS ${MMP_PRO_TOP_DIR}/Extension/openh264/libopenh264.a
    )
    add_library(Mmp::openH264 STATIC IMPORTED)
    set_target_properties(Mmp::openH264 PROPERTIES 
        IMPORTED_LOCATION ${MMP_PRO_TOP_DIR}/Extension/openh264/libopenh264.a
    )
    target_include_directories(Mmp::openH264 INTERFACE
      ${MMP_PRO_TOP_DIR}/Extension/openh264/codec/api
      ${MMP_PRO_TOP_DIR}/Extension/openh264/codec/api/wels
    )
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_library(Mmp::openH264 STATIC IMPORTED)
    set_target_properties(Mmp::openH264 PROPERTIES 
        IMPORTED_LOCATION ${MMP_PRO_TOP_DIR}/Extension/vcpkg/installed/x86-windows-static/lib/openh264.lib
    )
    target_include_directories(Mmp::openH264 INTERFACE
      ${MMP_PRO_TOP_DIR}/Extension/vcpkg/installed/x86-windows-static/include
      ${MMP_PRO_TOP_DIR}/Extension/vcpkg/installed/x86-windows-static/include/wels
    )
endif()
