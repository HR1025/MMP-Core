//
// SLCommon.h
//
// Library: SL
// Package: SL
// Module:  SL
// 

#include <string>
#include <iostream>

#include "Common/LogMessage.h"

#define SL_LOG_TRACE   MMP_MLOG_TRACE("GPU")
#define SL_LOG_DEBUG   MMP_MLOG_DEBUG("GPU")
#define SL_LOG_INFO    MMP_MLOG_INFO("GPU")
#define SL_LOG_WARN    MMP_MLOG_WARN("GPU")
#define SL_LOG_ERROR   MMP_MLOG_ERROR("GPU")
#define SL_LOG_FATAL   MMP_MLOG_FATAL("GPU")

namespace Mmp
{
namespace SL
{

enum class ShaderStage
{
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	COMPUTE,
};
const std::string ShaderStageToStr(ShaderStage stage);
extern std::ostream& operator<<(std::ostream& os, ShaderStage stage);

enum class ShaderLanguage
{
	GLSL_4xx,	   // OpenGL 4
	ELSL_3xx,      // OpenGL ES3
	GLSL_VULKAN,   // Vulkan
	HLSL_D3D9,     // Direct X
	HLSL_D3D11,
	HLSL_D3D12,
	MSL,           // Metal
};
const std::string ShaderLanguageToStr(ShaderLanguage sl);
extern std::ostream& operator<<(std::ostream& os, SL::ShaderLanguage sl);

} // namespace SL
} // namespace Mmp