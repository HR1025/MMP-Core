#include "RKUtil.h"

#include <cassert>

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

} // namespace Codec
} // namespace Mmp