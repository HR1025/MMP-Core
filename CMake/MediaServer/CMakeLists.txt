project(media_server)

if (USE_MEDIA_SERVER)
    set(MEDIA_SERVER_ROOT ${MMP_PRO_TOP_DIR}/Extension/media-server)

    set(MEDIA_SERVER_INC)
    set(MEDIA_SERVER_SRCS)
    set(MEDIA_SERVER_LIBS)

    if (USE_MEDIA_SERVER_MPEG)
    list(APPEND MEDIA_SERVER_SRCS
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include/mpeg-element-descriptor.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include/mpeg-muxer.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include/mpeg-proto.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include/mpeg-ps.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include/mpeg-types.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include/mpeg-util.h

        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-crc32.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-element-descriptor.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-muxer.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-pack-header.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-packet.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-pat.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-pes-internal.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-pes.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-pmt.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ps-dec.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ps-enc.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ps-internal.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-psd.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-psm.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-sdt.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-system-header.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ts-dec.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ts-enc.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ts-h264.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ts-h265.c
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ts-internal.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-ts-opus.h
        ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source/mpeg-util.c
    )
    endif()

    add_library(media_server STATIC ${MEDIA_SERVER_SRCS})

    if (USE_MEDIA_SERVER_MPEG)
    target_include_directories(media_server
        PUBLIC
            ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/include
        PRIVATE
            ${MMP_PRO_TOP_DIR}/Extension/media-server/libmpeg/source
    )
    endif()
endif()