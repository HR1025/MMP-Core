#include "VAUtil.h"

namespace Mmp
{
namespace Codec
{

/**
 * @sa https://github.com/intel/libva/blob/master/va/va.h
 */
std::string VAProfileToStr(VAProfile profile)
{
    switch (profile)
    {
        case VAProfileNone: return "VAProfileNone";
        case VAProfileMPEG2Simple: return "VAProfileMPEG2Simple";
        case VAProfileMPEG2Main: return "VAProfileMPEG2Main";
        case VAProfileMPEG4Simple: return "VAProfileMPEG4Simple";
        case VAProfileMPEG4AdvancedSimple: return "VAProfileMPEG4AdvancedSimple";
        case VAProfileMPEG4Main: return "VAProfileMPEG4Main";
        case 5 /* VAProfileH264Baseline */: return "VAProfileH264Baseline";
        case VAProfileH264Main: return "VAProfileH264Main";
        case VAProfileH264High: return "VAProfileH264High";
        case VAProfileVC1Simple: return "VAProfileVC1Simple";
        case VAProfileVC1Main: return "VAProfileVC1Main";
        case VAProfileVC1Advanced: return "VAProfileVC1Advanced";
        case VAProfileH263Baseline: return "VAProfileH263Baseline";
        case VAProfileH264ConstrainedBaseline: return "VAProfileH264ConstrainedBaseline";
        case VAProfileVP8Version0_3: return "VAProfileVP8Version0_3";
        case VAProfileH264MultiviewHigh: return "VAProfileH264MultiviewHigh";
        case VAProfileH264StereoHigh: return "VAProfileH264StereoHigh";
        case VAProfileHEVCMain: return "VAProfileHEVCMain";
        case VAProfileHEVCMain10: return "VAProfileHEVCMain10";
        case VAProfileVP9Profile0: return "VAProfileVP9Profile0";
        case VAProfileVP9Profile1: return "VAProfileVP9Profile1";
        case VAProfileVP9Profile2: return "VAProfileVP9Profile2";
        case VAProfileVP9Profile3: return "VAProfileVP9Profile3";
        case VAProfileHEVCMain12: return "VAProfileHEVCMain12";
        case VAProfileHEVCMain422_10: return "VAProfileHEVCMain422_10";
        case VAProfileHEVCMain422_12: return "VAProfileHEVCMain422_12";
        case VAProfileHEVCMain444: return "VAProfileHEVCMain444";
        case VAProfileHEVCMain444_10: return "VAProfileHEVCMain444_10";
        case VAProfileHEVCMain444_12: return "VAProfileHEVCMain444_12";
        case VAProfileHEVCSccMain: return "VAProfileHEVCSccMain";
        case VAProfileHEVCSccMain10: return "VAProfileHEVCSccMain10";
        case VAProfileHEVCSccMain444: return "VAProfileHEVCSccMain444";
        case VAProfileAV1Profile0: return "VAProfileAV1Profile0";
        case VAProfileAV1Profile1: return "VAProfileAV1Profile1";
#if 0
        case VAProfileHEVCSccMain444_10: return "VAProfileHEVCSccMain444_10";
        case VAProfileProtected: return "VAProfileProtected";
        case VAProfileH264High10: return "VAProfileH264High10";
#endif
        default:
            return "VAProfileUnknown";
    }
}

std::string VAStatusToStr(VAStatus status)
{
    switch (status)
    {
        case VA_STATUS_SUCCESS: return "VA_STATUS_SUCCESS";
        case VA_STATUS_ERROR_OPERATION_FAILED: return "VA_STATUS_ERROR_OPERATION_FAILED";
        case VA_STATUS_ERROR_ALLOCATION_FAILED: return "VA_STATUS_ERROR_ALLOCATION_FAILED";
        case VA_STATUS_ERROR_INVALID_DISPLAY: return "VA_STATUS_ERROR_INVALID_DISPLAY";
        case VA_STATUS_ERROR_INVALID_CONFIG: return "VA_STATUS_ERROR_INVALID_CONFIG";
        case VA_STATUS_ERROR_INVALID_CONTEXT: return "VA_STATUS_ERROR_INVALID_CONTEXT";
        case VA_STATUS_ERROR_INVALID_SURFACE: return "VA_STATUS_ERROR_INVALID_SURFACE";
        case VA_STATUS_ERROR_INVALID_BUFFER: return "VA_STATUS_ERROR_INVALID_BUFFER";
        case VA_STATUS_ERROR_INVALID_IMAGE: return "VA_STATUS_ERROR_INVALID_IMAGE";
        case VA_STATUS_ERROR_INVALID_SUBPICTURE: return "VA_STATUS_ERROR_INVALID_SUBPICTURE";
        case VA_STATUS_ERROR_ATTR_NOT_SUPPORTED: return "VA_STATUS_ERROR_ATTR_NOT_SUPPORTED";
        case VA_STATUS_ERROR_MAX_NUM_EXCEEDED: return "VA_STATUS_ERROR_MAX_NUM_EXCEEDED";
        case VA_STATUS_ERROR_UNSUPPORTED_PROFILE: return "VA_STATUS_ERROR_UNSUPPORTED_PROFILE";
        case VA_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT: return "VA_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT";
        case VA_STATUS_ERROR_UNSUPPORTED_RT_FORMAT: return "VA_STATUS_ERROR_UNSUPPORTED_RT_FORMAT";
        case VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: return "VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE";
        case VA_STATUS_ERROR_SURFACE_BUSY: return "VA_STATUS_ERROR_SURFACE_BUSY";
        case VA_STATUS_ERROR_FLAG_NOT_SUPPORTED: return "VA_STATUS_ERROR_FLAG_NOT_SUPPORTED";
        case VA_STATUS_ERROR_INVALID_PARAMETER: return "VA_STATUS_ERROR_INVALID_PARAMETER";
        case VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED: return "VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED";
        case VA_STATUS_ERROR_UNIMPLEMENTED: return "VA_STATUS_ERROR_UNIMPLEMENTED";
        case VA_STATUS_ERROR_SURFACE_IN_DISPLAYING: return "VA_STATUS_ERROR_SURFACE_IN_DISPLAYING";
        case VA_STATUS_ERROR_INVALID_IMAGE_FORMAT: return "VA_STATUS_ERROR_INVALID_IMAGE_FORMAT";
        case VA_STATUS_ERROR_DECODING_ERROR: return "VA_STATUS_ERROR_DECODING_ERROR";
        case VA_STATUS_ERROR_ENCODING_ERROR: return "VA_STATUS_ERROR_ENCODING_ERROR";
        case VA_STATUS_ERROR_INVALID_VALUE: return "VA_STATUS_ERROR_INVALID_VALUE";
        case VA_STATUS_ERROR_HW_BUSY: return "VA_STATUS_ERROR_HW_BUSY";
        case VA_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE: return "VA_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE";
        case VA_STATUS_ERROR_NOT_ENOUGH_BUFFER: return "VA_STATUS_ERROR_NOT_ENOUGH_BUFFER";
        case VA_STATUS_ERROR_TIMEDOUT: return "VA_STATUS_ERROR_TIMEDOUT";
        default:
            return std::string() + "VA_STATUS_ERROR_UNKNOWN(" + std::to_string((uint32_t)status) + ")";
    }
}

} // namespace Codec
} // namespace Mmp