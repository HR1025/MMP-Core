#include "RKUtil.h"

#include <cassert>
#include <cstdint>
#include <string>

namespace Mmp
{
namespace Codec
{

std::string RkMppRetToStr(MPP_RET ret)
{
    switch (ret) 
    {
        case MPP_RET::MPP_SUCCESS: return "MPP_SUCCESS";
        case MPP_RET::MPP_NOK: return "MPP_NOK";
        case MPP_RET::MPP_ERR_UNKNOW: return "MPP_ERR_UNKNOW";
        case MPP_RET::MPP_ERR_NULL_PTR: return "MPP_ERR_NULL_PTR";
        case MPP_RET::MPP_ERR_MALLOC: return "MPP_ERR_MALLOC";
        case MPP_RET::MPP_ERR_OPEN_FILE: return "MPP_ERR_OPEN_FILE";
        case MPP_RET::MPP_ERR_VALUE: return "MPP_ERR_VALUE";
        case MPP_RET::MPP_ERR_READ_BIT: return "MPP_ERR_READ_BIT";
        case MPP_RET::MPP_ERR_TIMEOUT: return "MPP_ERR_TIMEOUT";
        case MPP_RET::MPP_ERR_PERM: return "MPP_ERR_PERM";
        case MPP_RET::MPP_ERR_BASE: return "MPP_ERR_BASE";
        case MPP_RET::MPP_ERR_LIST_STREAM: return "MPP_ERR_LIST_STREAM";
        case MPP_RET::MPP_ERR_INIT: return "MPP_ERR_INIT";
        case MPP_RET::MPP_ERR_VPU_CODEC_INIT: return "MPP_ERR_VPU_CODEC_INIT";
        case MPP_RET::MPP_ERR_STREAM: return "MPP_ERR_STREAM";
        case MPP_RET::MPP_ERR_FATAL_THREAD: return "MPP_ERR_FATAL_THREAD";
        case MPP_RET::MPP_ERR_NOMEM: return "MPP_ERR_NOMEM";
        case MPP_RET::MPP_ERR_PROTOL: return "MPP_ERR_PROTOL"; 
        case MPP_RET::MPP_FAIL_SPLIT_FRAME: return "MPP_FAIL_SPLIT_FRAME";
        case MPP_RET::MPP_ERR_VPUHW: return "MPP_ERR_VPUHW";
        case MPP_RET::MPP_EOS_STREAM_REACHED: return "MPP_EOS_STREAM_REACHED";
        case MPP_RET::MPP_ERR_BUFFER_FULL: return "MPP_ERR_BUFFER_FULL";
        case MPP_RET::MPP_ERR_DISPLAY_FULL: return "MPP_ERR_DISPLAY_FULL";
        default:
            assert(false);
            return "MPP_UNKOWN";
    }
}

std::string MpiCmdToStr(MpiCmd cmd)
{
    switch (cmd) 
    {
        case MpiCmd::MPP_OSAL_CMD_BASE: return "MPP_OSAL_CMD_BASE";
        case MpiCmd::MPP_OSAL_CMD_END: return "MPP_OSAL_CMD_END";
        case MpiCmd::MPP_CMD_BASE: return "MPP_CMD_BASE";
        case MpiCmd::MPP_ENABLE_DEINTERLACE: return "MPP_ENABLE_DEINTERLACE";
        case MpiCmd::MPP_SET_INPUT_TIMEOUT: return "MPP_SET_INPUT_TIMEOUT";
        case MpiCmd::MPP_SET_OUTPUT_TIMEOUT: return "MPP_SET_OUTPUT_TIMEOUT";
        case MpiCmd::MPP_SET_DISABLE_THREAD: return "MPP_SET_DISABLE_THREAD";
        case MpiCmd::MPP_STATE_CMD_BASE: return "MPP_STATE_CMD_BASE";
        case MpiCmd::MPP_START: return "MPP_START";
        case MpiCmd::MPP_STOP: return "MPP_STOP";
        case MpiCmd::MPP_PAUSE: return "MPP_PAUSE";
        case MpiCmd::MPP_RESUME: return "MPP_RESUME";
        case MpiCmd::MPP_CMD_END: return "MPP_CMD_END";
        case MpiCmd::MPP_CODEC_CMD_BASE: return "MPP_CODEC_CMD_BASE";
        case MpiCmd::MPP_CODEC_GET_FRAME_INFO: return "MPP_CODEC_GET_FRAME_INFO";
        case MpiCmd::MPP_CODEC_CMD_END: return "MPP_CODEC_CMD_END";
        case MpiCmd::MPP_DEC_CMD_BASE: return "MPP_DEC_CMD_BASE";
        case MpiCmd::MPP_DEC_SET_FRAME_INFO: return "MPP_DEC_SET_FRAME_INFO";
        case MpiCmd::MPP_DEC_SET_EXT_BUF_GROUP: return "MPP_DEC_SET_EXT_BUF_GROUP";
        case MpiCmd::MPP_DEC_SET_INFO_CHANGE_READY: return "MPP_DEC_SET_INFO_CHANGE_READY";
        case MpiCmd::MPP_DEC_SET_PRESENT_TIME_ORDER: return "MPP_DEC_SET_PRESENT_TIME_ORDER";
        case MpiCmd::MPP_DEC_SET_PARSER_SPLIT_MODE: return "MPP_DEC_SET_PARSER_SPLIT_MODE";
        case MpiCmd::MPP_DEC_SET_PARSER_FAST_MODE: return "MPP_DEC_SET_PARSER_FAST_MODE";
        case MpiCmd::MPP_DEC_GET_STREAM_COUNT: return "MPP_DEC_GET_STREAM_COUNT";
        case MpiCmd::MPP_DEC_GET_VPUMEM_USED_COUNT: return "MPP_DEC_GET_VPUMEM_USED_COUNT";
        case MpiCmd::MPP_DEC_SET_VC1_EXTRA_DATA: return "MPP_DEC_SET_VC1_EXTRA_DATA";
        case MpiCmd::MPP_DEC_SET_OUTPUT_FORMAT: return "MPP_DEC_SET_OUTPUT_FORMAT";
        case MpiCmd::MPP_DEC_SET_DISABLE_ERROR: return "MPP_DEC_SET_DISABLE_ERROR";
        case MpiCmd::MPP_DEC_SET_IMMEDIATE_OUT: return "MPP_DEC_SET_IMMEDIATE_OUT";
        case MpiCmd::MPP_DEC_SET_ENABLE_DEINTERLACE: return "MPP_DEC_SET_ENABLE_DEINTERLACE";
        case MpiCmd::MPP_DEC_SET_ENABLE_FAST_PLAY: return "MPP_DEC_SET_ENABLE_FAST_PLAY";
        case MpiCmd::MPP_DEC_SET_DISABLE_THREAD: return "MPP_DEC_SET_DISABLE_THREAD";
        case MpiCmd::MPP_DEC_SET_MAX_USE_BUFFER_SIZE: return "MPP_DEC_SET_MAX_USE_BUFFER_SIZE";
        case MpiCmd::MPP_DEC_SET_ENABLE_MVC: return "MPP_DEC_SET_ENABLE_MVC";
        case MpiCmd::MPP_DEC_CMD_QUERY: return "MPP_DEC_CMD_QUERY";
        case MpiCmd::MPP_DEC_QUERY: return "MPP_DEC_QUERY";
        case MpiCmd::CMD_DEC_CMD_CFG: return "CMD_DEC_CMD_CFG";
        case MpiCmd::MPP_DEC_SET_CFG: return "MPP_DEC_SET_CFG";
        case MpiCmd::MPP_DEC_GET_CFG: return "MPP_DEC_GET_CFG";
        case MpiCmd::MPP_DEC_CMD_END: return "MPP_DEC_CMD_END";
        case MpiCmd::MPP_ENC_CMD_BASE: return "MPP_ENC_CMD_BASE";
        case MpiCmd::MPP_ENC_SET_CFG: return "MPP_ENC_SET_CFG";
        case MpiCmd::MPP_ENC_GET_CFG: return "MPP_ENC_GET_CFG";
        case MpiCmd::MPP_ENC_SET_PREP_CFG: return "MPP_ENC_SET_PREP_CFG";
        case MpiCmd::MPP_ENC_GET_PREP_CFG: return "MPP_ENC_GET_PREP_CFG";
        case MpiCmd::MPP_ENC_SET_RC_CFG: return "MPP_ENC_SET_RC_CFG";
        case MpiCmd::MPP_ENC_GET_RC_CFG: return "MPP_ENC_GET_RC_CFG";
        case MpiCmd::MPP_ENC_SET_CODEC_CFG: return "MPP_ENC_SET_CODEC_CFG";
        case MpiCmd::MPP_ENC_GET_CODEC_CFG: return "MPP_ENC_GET_CODEC_CFG";
        case MpiCmd::MPP_ENC_SET_IDR_FRAME: return "MPP_ENC_SET_IDR_FRAME";
        case MpiCmd::MPP_ENC_SET_OSD_LEGACY_0: return "MPP_ENC_SET_OSD_LEGACY_0";
        case MpiCmd::MPP_ENC_SET_OSD_LEGACY_1: return "MPP_ENC_SET_OSD_LEGACY_1";
        case MpiCmd::MPP_ENC_SET_OSD_LEGACY_2: return "MPP_ENC_SET_OSD_LEGACY_2";
        case MpiCmd::MPP_ENC_GET_HDR_SYNC: return "MPP_ENC_GET_HDR_SYNC";
        case MpiCmd::MPP_ENC_GET_EXTRA_INFO: return "MPP_ENC_GET_EXTRA_INFO";
        case MpiCmd::MPP_ENC_SET_SEI_CFG: return "MPP_ENC_SET_SEI_CFG";
        case MpiCmd::MPP_ENC_GET_SEI_DATA: return "MPP_ENC_GET_SEI_DATA";
        case MpiCmd::MPP_ENC_PRE_ALLOC_BUFF: return "MPP_ENC_PRE_ALLOC_BUFF";
        case MpiCmd::MPP_ENC_SET_QP_RANGE: return "MPP_ENC_SET_QP_RANGE";
        case MpiCmd::MPP_ENC_SET_ROI_CFG: return "MPP_ENC_SET_ROI_CFG";
        case MpiCmd::MPP_ENC_SET_CTU_QP: return "MPP_ENC_SET_CTU_QP";
        case MpiCmd::MPP_ENC_CMD_QUERY: return "MPP_ENC_CMD_QUERY";
        case MpiCmd::MPP_ENC_QUERY: return "MPP_ENC_QUERY";
        case MpiCmd::MPP_ENC_CFG_RC_API: return "MPP_ENC_CFG_RC_API";
        case MpiCmd::MPP_ENC_GET_RC_API_ALL: return "MPP_ENC_GET_RC_API_ALL";
        case MpiCmd::MPP_ENC_GET_RC_API_BY_TYPE: return "MPP_ENC_GET_RC_API_BY_TYPE";
        case MpiCmd::MPP_ENC_SET_RC_API_CFG: return "MPP_ENC_SET_RC_API_CFG";
        case MpiCmd::MPP_ENC_GET_RC_API_CURRENT: return "MPP_ENC_GET_RC_API_CURRENT";
        case MpiCmd::MPP_ENC_SET_RC_API_CURRENT: return "MPP_ENC_SET_RC_API_CURRENT";
        case MpiCmd::MPP_ENC_CFG_MISC: return "MPP_ENC_CFG_MISC";
        case MpiCmd::MPP_ENC_SET_HEADER_MODE: return "MPP_ENC_SET_HEADER_MODE";
        case MpiCmd::MPP_ENC_GET_HEADER_MODE: return "MPP_ENC_GET_HEADER_MODE";
        case MpiCmd::MPP_ENC_CFG_SPLIT: return "MPP_ENC_CFG_SPLIT";
        case MpiCmd::MPP_ENC_SET_SPLIT: return "MPP_ENC_SET_SPLIT";
        case MpiCmd::MPP_ENC_GET_SPLIT: return "MPP_ENC_GET_SPLIT";
        case MpiCmd::MPP_ENC_CFG_REF: return "MPP_ENC_CFG_REF";
        case MpiCmd::MPP_ENC_SET_REF_CFG: return "MPP_ENC_SET_REF_CFG";
        case MpiCmd::MPP_ENC_CFG_OSD: return "MPP_ENC_CFG_OSD";
        case MpiCmd::MPP_ENC_SET_OSD_PLT_CFG: return "MPP_ENC_SET_OSD_PLT_CFG";
        case MpiCmd::MPP_ENC_GET_OSD_PLT_CFG: return "MPP_ENC_GET_OSD_PLT_CFG";
        case MpiCmd::MPP_ENC_CMD_END: return "MPP_ENC_CMD_END";
        case MpiCmd::MPP_ISP_CMD_BASE: return "MPP_ISP_CMD_BASE";
        case MpiCmd::MPP_ISP_CMD_END: return "MPP_ISP_CMD_END";
        case MpiCmd::MPP_HAL_CMD_BASE: return "MPP_HAL_CMD_BASE";
        case MpiCmd::MPP_HAL_CMD_END: return "MPP_HAL_CMD_END";
        case MpiCmd::MPI_CMD_BUTT: return "MPI_CMD_BUTT";
        default:
            return std::string("MPP_CMD_UNKONW(") + std::to_string(uint32_t(cmd)) + ")";
    }
}

} // namespace Codec
} // namespace Mmp