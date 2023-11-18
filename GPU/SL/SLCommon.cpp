#include "SLCommon.h"

#include <cassert>

namespace Mmp
{
namespace SL
{

const std::string ShaderStageToStr(ShaderStage stage)
{
    switch (stage)
    {
        case ShaderStage::VERTEX: return "VERTEX";
        case ShaderStage::FRAGMENT: return "FRAGMENT";
        case ShaderStage::GEOMETRY: return "GEOMETRY";
        case ShaderStage::COMPUTE: return "COMPUTE";
        default: assert(false); return "UNKNOWN";
    }
}


std::ostream& operator<<(std::ostream& os, ShaderStage stage)
{
    os << ShaderStageToStr(stage);
    return os;
}

const std::string ShaderLanguageToStr(ShaderLanguage sl)
{
    switch (sl)
    {
        case ShaderLanguage::GLSL_4xx: return "GLSL_4xx";
        case ShaderLanguage::ELSL_3xx: return "ELSL_3xx";
        case ShaderLanguage::GLSL_VULKAN: return "GLSL_VULKAN";
        case ShaderLanguage::HLSL_D3D9: return "HLSL_D3D9";
        case ShaderLanguage::HLSL_D3D11: return "HLSL_D3D11";\
        case ShaderLanguage::HLSL_D3D12: return "HLSL_D3D11";
        case ShaderLanguage::MSL: return "MSL";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, ShaderLanguage sl)
{
    os << ShaderLanguageToStr(sl);
    return os;
}

} // namespace SL
} // namespace Mmp