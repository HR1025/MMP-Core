project(mmp_openh264)

# Hint : 根据 OpenH264 meson 编写
# Todo : 适配除 x86, arm64 以外的架构平台

set(MMP_OPENH264_ROOT ${MMP_CORE_PRO_TOP_DIR}/Extension/openh264)

set(MMP_OPENH264_BASIC_INC)

list(APPEND MMP_OPENH264_BASIC_INC
    ${MMP_OPENH264_ROOT}/codec/api/wels
    ${MMP_OPENH264_ROOT}/codec/common/inc
)

if (MSVC)
    if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
        add_definitions(-DWIN64)
    elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
        add_definitions(-DHAVE_NEON_AARCH64)
    endif()
else()
    if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
        add_definitions(-DX86_32)
        add_definitions(-DHAVE_AVX2)
        add_definitions(-DX86_ASM)
        add_definitions(-DX86_32_ASM)
    elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
        add_definitions(-DHAVE_NEON_ARM64)
    endif()
endif()

if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
    list(APPEND MMP_OPENH264_BASIC_INC
        ${MMP_OPENH264_ROOT}/codec/common/x86
    )
elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
    list(APPEND MMP_OPENH264_COMMON_SRCS
        ${MMP_OPENH264_ROOT}/codec/common/arm
    )
endif()

##########################################################################
#                        Common (Begin)                                  #
##########################################################################
set(MMP_OPENH264_COMMON_INC)
set(MMP_OPENH264_COMMON_SRCS)
set(MMP_OPENH264_COMMON_LIBS)

list(APPEND MMP_OPENH264_COMMON_INC
    ${MMP_OPENH264_BASIC_INC}
)

list(APPEND MMP_OPENH264_COMMON_SRCS
    ${MMP_OPENH264_ROOT}/codec/common/src/common_tables.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/copy_mb.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/cpu.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/crt_util_safe_x.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/deblocking_common.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/expand_pic.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/intra_pred_common.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/mc.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/memory_align.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/sad_common.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/utils.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/welsCodecTrace.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/WelsTaskThread.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/WelsThread.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/WelsThreadLib.cpp
    ${MMP_OPENH264_ROOT}/codec/common/src/WelsThreadPool.cpp
)

if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
    list(APPEND MMP_OPENH264_COMMON_SRCS
        ${MMP_OPENH264_ROOT}/codec/common/x86/cpuid.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/dct.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/deblock.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/expand_picture.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/intra_pred_com.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/mb_copy.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/mc_chroma.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/mc_luma.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/satd_sad.asm
        ${MMP_OPENH264_ROOT}/codec/common/x86/vaa.asm
    )
elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
    list(APPEND MMP_OPENH264_COMMON_SRCS
        ${MMP_OPENH264_ROOT}/codec/common/arm64/copy_mb_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/common/arm64/deblocking_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/common/arm64/expand_picture_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/common/arm64/intra_pred_common_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/common/arm64/mc_aarch64_neon.S
    )
endif()
add_library(MMP_OPENH264_COMMON OBJECT ${MMP_OPENH264_COMMON_SRCS})

target_include_directories(MMP_OPENH264_COMMON PUBLIC ${MMP_OPENH264_COMMON_INC})
add_library(OpenH264::Common ALIAS MMP_OPENH264_COMMON)

##########################################################################
#                        Common (End)                                   #
##########################################################################


##########################################################################
#                        Decoder (Begin)                                  #
##########################################################################
set(MMP_OPENH264_DECODER_INC)
set(MMP_OPENH264_DECODER_SRCS)
set(MMP_OPENH264_DECODER_LIBS)

list(APPEND MMP_OPENH264_DECODER_INC
    ${MMP_OPENH264_BASIC_INC}
    ${MMP_OPENH264_ROOT}/codec/decoder/core/inc
    ${MMP_OPENH264_ROOT}/codec/decoder/plus/inc
)

list(APPEND MMP_OPENH264_DECODER_SRCS
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/au_parser.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/bit_stream.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/cabac_decoder.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/deblocking.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/decode_mb_aux.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/decode_slice.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/decoder.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/decoder_core.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/decoder_data_tables.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/error_concealment.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/fmo.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/get_intra_predictor.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/manage_dec_ref.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/memmgr_nal_unit.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/mv_pred.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/parse_mb_syn_cabac.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/parse_mb_syn_cavlc.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/pic_queue.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/rec_mb.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/plus/src/welsDecoderExt.cpp
    ${MMP_OPENH264_ROOT}/codec/decoder/core/src/wels_decoder_thread.cpp
)

if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
    list(APPEND MMP_OPENH264_DECODER_SRCS
        ${MMP_OPENH264_ROOT}/codec/decoder/core/x86/dct.asm
        ${MMP_OPENH264_ROOT}/codec/decoder/core/x86/intra_pred.asm
    )
elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
    list(APPEND MMP_OPENH264_DECODER_SRCS
        ${MMP_OPENH264_ROOT}/codec/decoder/core/arm64/block_add_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/decoder/core/arm64/intra_pred_aarch64_neon.S
    )
endif()
add_library(MMP_OPENH264_DECODER OBJECT ${MMP_OPENH264_DECODER_SRCS})

target_include_directories(MMP_OPENH264_DECODER PUBLIC ${MMP_OPENH264_DECODER_INC})
add_library(OpenH264::Decoder ALIAS MMP_OPENH264_DECODER)

##########################################################################
#                        Decoder (End)                                   #
##########################################################################

##########################################################################
#                        Process (Begin)                                  #
##########################################################################
set(MMP_OPENH264_PROCESS_INC)
set(MMP_OPENH264_PROCESS_SRCS)
set(MMP_OPENH264_PROCESS_LIBS)

list(APPEND MMP_OPENH264_PROCESS_INC
    ${MMP_OPENH264_BASIC_INC}
    ${MMP_OPENH264_ROOT}/codec/processing/interface
    ${MMP_OPENH264_ROOT}/codec/processing/src/common
    ${MMP_OPENH264_ROOT}/codec/processing/src/adaptivequantization
    ${MMP_OPENH264_ROOT}/codec/processing/src/downsample
    ${MMP_OPENH264_ROOT}/codec/processing/src/scrolldetection
    ${MMP_OPENH264_ROOT}/codec/processing/src/vaacalc
)

list(APPEND MMP_OPENH264_PROCESS_SRCS
    ${MMP_OPENH264_ROOT}/codec/processing/src/adaptivequantization/AdaptiveQuantization.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/backgrounddetection/BackgroundDetection.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/common/memory.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/common/WelsFrameWork.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/common/WelsFrameWorkEx.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/complexityanalysis/ComplexityAnalysis.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/denoise/denoise.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/denoise/denoise_filter.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/downsample/downsample.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/downsample/downsamplefuncs.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/imagerotate/imagerotate.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/imagerotate/imagerotatefuncs.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/scenechangedetection/SceneChangeDetection.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/scrolldetection/ScrollDetection.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/scrolldetection/ScrollDetectionFuncs.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/vaacalc/vaacalcfuncs.cpp
    ${MMP_OPENH264_ROOT}/codec/processing/src/vaacalc/vaacalculation.cpp
)

if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
    list(APPEND MMP_OPENH264_PROCESS_SRCS
        ${MMP_OPENH264_ROOT}/codec/processing/src/x86/denoisefilter.asm
        ${MMP_OPENH264_ROOT}/codec/processing/src/x86/downsample_bilinear.asm
        ${MMP_OPENH264_ROOT}/codec/processing/src/x86/vaa.asm
    )
elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
    list(APPEND MMP_OPENH264_PROCESS_SRCS
        ${MMP_OPENH264_ROOT}/codec/processing/src/arm64/adaptive_quantization_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/processing/src/arm64/down_sample_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/processing/src/arm64/pixel_sad_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/processing/src/arm64/vaa_calc_aarch64_neon.S
    )
endif()
add_library(MMP_OPENH264_PROCESS OBJECT ${MMP_OPENH264_PROCESS_SRCS})

target_include_directories(MMP_OPENH264_PROCESS PUBLIC ${MMP_OPENH264_PROCESS_INC})
add_library(OpenH264::Process ALIAS MMP_OPENH264_PROCESS)

##########################################################################
#                        Process (End)                                  #
##########################################################################

##########################################################################
#                        Encoder (Begin)                                  #
##########################################################################
set(MMP_OPENH264_ENCODER_INC)
set(MMP_OPENH264_ENCODER_SRCS)
set(MMP_OPENH264_ENCODER_LIBS)

list(APPEND MMP_OPENH264_ENCODER_INC
    ${MMP_OPENH264_BASIC_INC}
    ${MMP_OPENH264_PROCESS_INC}
    ${MMP_OPENH264_ROOT}/codec/encoder/core/inc
    ${MMP_OPENH264_ROOT}/codec/encoder/plus/inc
)

list(APPEND MMP_OPENH264_ENCODER_SRCS
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/au_set.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/deblocking.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/decode_mb_aux.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/encode_mb_aux.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/encoder.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/encoder_data_tables.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/encoder_ext.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/get_intra_predictor.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/md.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/mv_pred.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/nal_encap.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/paraset_strategy.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/picture_handle.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/ratectl.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/ref_list_mgr_svc.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/sample.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/set_mb_syn_cabac.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/set_mb_syn_cavlc.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/slice_multi_threading.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_base_layer_md.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_enc_slice_segment.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_encode_mb.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_encode_slice.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_mode_decision.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_motion_estimate.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_set_mb_syn_cabac.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/svc_set_mb_syn_cavlc.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/wels_preprocess.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/wels_task_base.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/wels_task_encoder.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/core/src/wels_task_management.cpp
    ${MMP_OPENH264_ROOT}/codec/encoder/plus/src/welsEncoderExt.cpp
)

if (${MMP_SYSTEM_DEVICE} MATCHES "X86")
    list(APPEND MMP_OPENH264_ENCODER_SRCS
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/coeff.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/dct.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/intra_pred.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/matrix_transpose.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/memzero.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/quant.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/sample_sc.asm
        ${MMP_OPENH264_ROOT}/codec/encoder/core/x86/score.asm
    )
elseif (${MMP_SYSTEM_DEVICE} MATCHES "ARM64")
    list(APPEND MMP_OPENH264_ENCODER_SRCS
        ${MMP_OPENH264_ROOT}/codec/encoder/core/arm64/intra_pred_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/encoder/core/arm64/intra_pred_sad_3_opt_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/encoder/core/arm64/memory_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/encoder/core/arm64/pixel_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/encoder/core/arm64/reconstruct_aarch64_neon.S
        ${MMP_OPENH264_ROOT}/codec/encoder/core/arm64/svc_motion_estimation_aarch64_neon.S
    )
endif()
add_library(MMP_OPENH264_ENCODER OBJECT ${MMP_OPENH264_ENCODER_SRCS})

target_include_directories(MMP_OPENH264_ENCODER PUBLIC ${MMP_OPENH264_ENCODER_INC})
add_library(OpenH264::Encoder ALIAS MMP_OPENH264_ENCODER)

##########################################################################
#                        Encoder (End)                                   #
##########################################################################


##########################################################################
#                        OpenH264 (Begin)                                #
##########################################################################
add_library(MMP_OPENH264 STATIC)
target_include_directories(MMP_OPENH264 PUBLIC ${MMP_OPENH264_BASIC_INC})
target_link_libraries(MMP_OPENH264 PUBLIC OpenH264::Common OpenH264::Decoder OpenH264::Process OpenH264::Encoder)
add_library(Mmp::OpenH264 ALIAS MMP_OPENH264)
##########################################################################
#                        OpenH264 (End)                                  #
##########################################################################