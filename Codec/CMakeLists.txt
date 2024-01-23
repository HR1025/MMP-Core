cmake_minimum_required(VERSION 3.8)

set(Codec_SRCS)
set(Codec_INCS)
set(Codec_LIB_DIR)
set(Codec_PRI_LIBS)
set(Codec_PUB_LIBS)

option(ENBALE_H26x_DEBUG "H26x 子仓库调试模式" OFF)

list(APPEND Codec_PRI_LIBS
    Poco::Foundation 
    Mmp::Common
)

list(APPEND Codec_INCS
    ${MMP_CORE_PRO_TOP_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
list(APPEND Codec_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/CodecCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CodecCommon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractEncoder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractDecorder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CodecFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CodecFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CodecConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CodecConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StreamFrame.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StreamFrame.cpp
)

# VAAPI
if (USE_VAAPI)
    if (NOT LIBVA_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(LIBVA REQUIRED libva)
    endif()
    message("-- Found LIBVA")
    message("  -- LIBVA INCS : ${LIBVA_INCLUDE_DIRS}")
    message("  -- LIBVA LIB DIR : ${LIBVA_LIBRARY_DIRS}")
    message("  -- LIBVA LIBS : ${LIBVA_LIBRARIES}")
    list(APPEND Codec_INCS ${LIBVA_INCLUDE_DIRS})
    list(APPEND Codec_LIB_DIR ${LIBVA_LIBRARY_DIRS})
    list(APPEND Codec_PUB_LIBS ${LIBVA_LIBRARIES})
    list(APPEND Codec_PUB_LIBS va-drm)
    ## drm
    if (NOT DRM_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(DRM REQUIRED libdrm)
    endif()
    message("-- Found DRM")
    message("  -- DRM INCS : ${DRM_INCLUDE_DIRS}")
    message("  -- DRM LIB DIR : ${DRM_LIBRARY_DIRS}")
    message("  -- DRM LIBS : ${DRM_LIBRARIES}")
    list(APPEND Codec_INCS ${DRM_INCLUDE_DIRS})
    list(APPEND Codec_LIB_DIR ${DRM_LIBRARY_DIRS})
    list(APPEND Codec_PUB_LIBS ${DRM_LIBRARIES})
endif()

# Rockchip
if (USE_ROCKCHIP)
    if (NOT RKMPP_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(RKMPP REQUIRED rockchip_mpp)
    endif()
    message("-- Found RKMPP")
    message("  -- RKMPP INCS : ${RKMPP_INCLUDE_DIRS}")
    message("  -- RKMPP LIB DIR : ${RKMPP_LIBRARY_DIRS}")
    message("  -- RKMPP LIBS : ${RKMPP_LIBRARIES}")
    list(APPEND Codec_INCS ${RKMPP_INCLUDE_DIRS})
    list(APPEND Codec_LIB_DIR ${RKMPP_LIBRARY_DIRS})
    list(APPEND Codec_PUB_LIBS ${RKMPP_LIBRARIES})
    if (NOT RKVPU_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(RKVPU REQUIRED rockchip_vpu) 
    endif()
    message("-- Found RKVPU")
    message("  -- RKVPU INCS : ${RKVPU_INCLUDE_DIRS}")
    message("  -- RKVPU LIB DIR : ${RKVPU_LIBRARY_DIRS}")
    message("  -- RKVPU LIBS : ${RKVPU_LIBRARIES}")
    list(APPEND Codec_INCS ${RKVPU_INCLUDE_DIRS})
    list(APPEND Codec_LIB_DIR ${RKVPU_LIBRARY_DIRS})
    list(APPEND Codec_PUB_LIBS ${RKVPU_LIBRARIES})
endif()

if (USE_OPENH264)
    list(APPEND Codec_PUB_LIBS Mmp::OpenH264)
endif()

# H264
list(APPEND Codec_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}/H264
)
list(APPEND Codec_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/AbstractH26xByteReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H26xBinaryReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H26xBinaryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H26xUltis.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H26xUltis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H264Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H264Common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H264Deserialize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H264Deserialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H264SliceDecodingProcess.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H264SliceDecodingProcess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H265Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H265Common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H265Deserialize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264/H265Deserialize.cpp
)

# OpenH264
if (USE_OPENH264)
    list(APPEND Codec_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/openh264/OpenH264Common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/openh264/OpenH264Common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/openh264/OpenH264Util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/openh264/OpenH264Util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/openh264/OpenH264Decoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/openh264/OpenH264Decoder.cpp
    )
endif()


# VAAPI
if (USE_VAAPI)
    list(APPEND Codec_INCS
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI
    )
    list(APPEND Codec_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VACommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VACommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VAUtil.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VAUtil.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VADevice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VADevice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VATranslator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VATranslator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VADecoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VADecoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VADecodePictureContext.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VADecodePictureContext.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VaH264SliceDecodingProcess.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VaH264SliceDecodingProcess.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VAH264Decoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VAH264Decoder.cpp
    )
endif()

# Rockchip
if (USE_ROCKCHIP)
    list(APPEND Codec_INCS
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip
    )
    list(APPEND Codec_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKCommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKUtil.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKUtil.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKTranslator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKTranslator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKDecoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKDecoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKH264Decoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Rockchip/RKH264Decoder.cpp
    )
endif()

# PNG
list(APPEND Codec_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG
)
list(APPEND Codec_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGCommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGUtil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGUtil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGDecorder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGDecorder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGEncoder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PNG/PNGEncoder.cpp
)
list(APPEND Codec_PUB_LIBS
    lodepng
)

add_library(MMP_Codec STATIC ${Codec_SRCS})
add_library(Mmp::Codec ALIAS MMP_Codec)
target_include_directories(MMP_Codec PUBLIC ${Codec_INCS})
target_link_libraries(MMP_Codec PUBLIC ${Codec_PUB_LIBS} PRIVATE ${Codec_PRI_LIBS}) 

list(LENGTH Codec_LIB_DIR Codec_LIB_DIR_LEN)
if (NOT(${Codec_LIB_DIR_LEN} EQUAL 0))
    target_link_directories(MMP_Codec PUBLIC ${Codec_LIB_DIR})
endif()
if (ENBALE_H26x_DEBUG)
    target_compile_definitions(MMP_Codec PRIVATE MMP_H26X_DEBUG_MODE)
    target_compile_definitions(MMP_Codec PRIVATE ENABLE_MMP_SD_DEBUG)
    target_compile_definitions(MMP_Codec PRIVATE MMP_H26x_EXTERN_HEADER="Codec/CodecCommon.h")
    target_compile_definitions(MMP_Codec PRIVATE H26x_LOG_TERMINATOR="") # MMP 日志会自动换行, 故换行符并不需要
    target_compile_definitions(MMP_Codec PRIVATE H26x_LOG_INFO=CODEC_LOG_INFO)
    target_compile_definitions(MMP_Codec PRIVATE H26x_LOG_ERROR=CODEC_LOG_ERROR)
endif()
if (USE_VAAPI)
    target_compile_definitions(MMP_Codec PRIVATE USE_VAAPI)
endif()
if (USE_X11)
    target_compile_definitions(MMP_Codec PRIVATE USE_X11)
endif()
if (USE_OPENH264)
    target_compile_definitions (MMP_Codec PRIVATE USE_OPENH264)
endif()
if (USE_ROCKCHIP)
    target_compile_definitions(MMP_Codec PRIVATE USE_ROCKCHIP)
endif()