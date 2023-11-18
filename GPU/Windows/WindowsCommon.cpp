#include "WindowsCommon.h"

#include <cassert>

namespace Mmp
{

const std::string APITypeToStr(APIType type)
{
    switch (type)
    {
        case APIType::OPENGL: return "OPENGL";
        case APIType::OPENGL_ES3: return "OPENGL_ES3";
        case APIType::D3D11: return "D3D11";
        case APIType::VULKAN: return "VULKAN";
        case APIType::UNKNOWN: return "UNKNOWN";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, APIType type)
{
    os << APITypeToStr(type);
    return os;
}

WindowInfo::WindowInfo()
{
    apiType = APIType::OPENGL;
    vMajor  = 0;
    vMinor  = 0;
}

} // namespace Mmp