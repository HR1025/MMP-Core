cmake_minimum_required(VERSION 3.8)

set(Codec_SRCS)
set(Codec_INCS)
set(Codec_LIB_DIR)
set(Codec_LIBS)

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
    list(APPEND Codec_LIBS ${LIBVA_LIBRARIES})
    list(APPEND Codec_LIBS va-drm)
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
    list(APPEND Codec_LIBS ${DRM_LIBRARIES})
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
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VAH264Decoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/VAAPI/VAH264Decoder.cpp
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
list(APPEND Codec_LIBS
    lodepng
)

add_library(MMP_Codec OBJECT ${Codec_SRCS})
add_library(Mmp::Codec ALIAS MMP_Codec)
target_include_directories(MMP_Codec PUBLIC ${Codec_INCS})
target_link_libraries(MMP_Codec PUBLIC ${Codec_LIBS} PRIVATE Poco::Foundation Mmp::Common) 

list(LENGTH Codec_LIB_DIR Codec_LIB_DIR_LEN)
if (NOT(${Codec_LIB_DIR_LEN} EQUAL 0))
    target_link_directories(MMP_Codec PUBLIC ${Codec_LIB_DIR})
endif()
if (USE_VAAPI)
    target_compile_definitions(MMP_Codec PRIVATE USE_VAAPI)
endif()
if (USE_X11)
    target_compile_definitions(MMP_Codec PRIVATE USE_X11)
endif()