//
// GLCommon.h
//
// Library: GPU
// Package: GL
// Module:  GL
// 

#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "Common/Common.h"
#include "Common/LogMessage.h"
#include "Common/NormalSharedData.h"
#include "GPU/SL/SLCommon.h"

#define GL_LOG_TRACE   MMP_MLOG_TRACE("GPU")
#define GL_LOG_DEBUG   MMP_MLOG_DEBUG("GPU")
#define GL_LOG_INFO    MMP_MLOG_INFO("GPU")
#define GL_LOG_WARN    MMP_MLOG_WARN("GPU")
#define GL_LOG_ERROR   MMP_MLOG_ERROR("GPU")
#define GL_LOG_FATAL   MMP_MLOG_FATAL("GPU")

namespace Mmp
{

enum class GPUBackend
{
	OPENGL,
	OPENGL_ES,
	D3D11
};
const std::string GPUBackendToStr(GPUBackend backend);
extern std::ostream& operator<<(std::ostream& os, GPUBackend backend);

enum class GpuTaskStatus
{
	NORMAL,
	PRESENT,
	EXIT
};
const std::string GpuTaskStatusToStr(GpuTaskStatus status);
extern std::ostream& operator<<(std::ostream& os, GpuTaskStatus status);

enum class DataFormat
{
	UNDEFINED,

	R8_UNORM,
	R8G8_UNORM,
	R8G8B8_UNORM,

	Y8_UNORM,
	U8_UNORM,
	V8_UNORM,
	U8V8_UNORM,

	R8G8B8A8_UNORM,
	R8G8B8A8_UNORM_SRGB,
	B8G8R8A8_UNORM,  // D3D style
	B8G8R8A8_UNORM_SRGB,  // D3D style

	NV12_UINT,
	R8G8B8A8_SNORM,
	R8G8B8A8_UINT,
	R8G8B8A8_SINT,

	R4G4_UNORM_PACK8,
	A4R4G4B4_UNORM_PACK16,  // A4 in the UPPER bit
	B4G4R4A4_UNORM_PACK16,
	R4G4B4A4_UNORM_PACK16,
	R5G6B5_UNORM_PACK16,
	B5G6R5_UNORM_PACK16,
	R5G5B5A1_UNORM_PACK16, // A1 in the LOWER bit
	B5G5R5A1_UNORM_PACK16, // A1 in the LOWER bit
	A1R5G5B5_UNORM_PACK16, // A1 in the UPPER bit.
	A1B5G5R5_UNORM_PACK16, // A1 in the UPPER bit. OpenGL-only.

	R16_UNORM,

	R16_FLOAT,
	R16G16_FLOAT,
	R16G16B16A16_FLOAT,

	R32_FLOAT,
	R32G32_FLOAT,
	R32G32B32_FLOAT,
	R32G32B32A32_FLOAT,

	// Block compression formats.
	// These are modern names for DXT and friends, now patent free.
	// https://msdn.microsoft.com/en-us/library/bb694531.aspx
	BC1_RGBA_UNORM_BLOCK,
	BC1_RGBA_SRGB_BLOCK,
	BC2_UNORM_BLOCK,  // 4-bit straight alpha + DXT1 color. Usually not worth using
	BC2_SRGB_BLOCK,
	BC3_UNORM_BLOCK,  // 3-bit alpha with 2 ref values (+ magic) + DXT1 color
	BC3_SRGB_BLOCK,
	BC4_UNORM_BLOCK,  // 1-channel, same storage as BC3 alpha
	BC4_SNORM_BLOCK,
	BC5_UNORM_BLOCK,  // 2-channel RG, each has same storage as BC3 alpha
	BC5_SNORM_BLOCK,
	BC6H_UFLOAT_BLOCK,  // TODO
	BC6H_SFLOAT_BLOCK,
	BC7_UNORM_BLOCK,    // Highly advanced, very expensive to compress, very good quality.
	BC7_SRGB_BLOCK,

	ETC1,

	S8,
	D16,
	D16_S8,
	D24_S8,
	D32F,
	D32F_S8,
};
const std::string DataFormatToStr(DataFormat format);
extern std::ostream& operator<<(std::ostream& os, DataFormat format);

enum SupportFormat
{
	SF_RENDERTARGET   = 1 << 1u,
	SF_TEXTURE        = 1 << 2u,
	SF_INPUTLAYOUT    = 1 << 3u,
	SF_DEPTHSTENCIL   = 1 << 4u,
	SF_AUTOGEN_MIPS   = 1 << 5u,
	SF_BLIT           = 1 << 6u,
	SF_STORAGE_IMAGE  = 1 << 7u
};

enum GlTextureFlags : uint64_t
{
	NONE = 0u,
	TEXTURE_USE_FOR_RENDER = 1 << 1u,        // 是否用于渲染   (D3D)
	TEXTURE_EXTERNAL       = 1 << 2u,        // 是否为外部导入 (OpenGL、OpenGLES)
	TEXTURE_YUV            = 1 << 3u,        // 是否为YUV纹理  (OpenGLES)
};

enum class TextureFilter
{
	NEAREST,
	LINEAR,
};
const std::string TextureFilterToStr(TextureFilter tf);
extern std::ostream& operator<<(std::ostream& os, TextureFilter tf);

/**
 * @sa GLTextureAddressModeToOpenGLType
 */
enum class TextureAddressMode 
{
	REPEAT,
	REPEAT_MIRROR,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
};
const std::string TextureAddressModeToStr(TextureAddressMode mode);
extern std::ostream& operator<<(std::ostream& os, TextureAddressMode mode);

/**
 * @sa GLTextureTypeToOpenGLType
 */
enum class TextureType
{
	LINEAR1D,
	LINEAR2D,
	LINEAR3D,
	CUBE,
	ARRAY1D,
	ARRAY2D,
	UNKNOWN,
};
const std::string TextureTypeToStr(TextureType type);
extern std::ostream& operator<<(std::ostream& os, TextureType type);

/**
 * @sa GLComparisonTypeToOpenGLType
 */
enum class Comparison
{
	NEVER,
	LESS,
	EQUAL,
	LESS_EQUAL,
	GREATER,
	NOT_EQUAL,
	GREATER_EQUAL,
	ALWAYS,
};
const std::string ComparisonToStr(Comparison Comparison);
extern std::ostream& operator<<(std::ostream& os, Comparison Comparison);

enum class LogicOp
{
	CLEAR,
	SET,
	COPY,
	COPY_INVERTED,
	NOOP,
	INVERT,
	AND,
	NAND,
	OR,
	NOR,
	XOR,
	EQUIV,
	AND_REVERSE,
	AND_INVERTED,
	OR_REVERSE,
	OR_INVERTED,
};
const std::string LogicOpToStr(LogicOp op);
extern std::ostream& operator<<(std::ostream& os, LogicOp op);

/**
 * @sa GLStencilOpToOpenGLType
 */
enum class StencilOp
{
	KEEP,
	ZERO,
	REPLACE,
	INCREMENT_AND_CLAMP,
	DECREMENT_AND_CLAMP,
	INVERT,
	INCREMENT_AND_WRAP,
	DECREMENT_AND_WRAP,
};
const std::string StencilOpToStr(StencilOp op);
extern std::ostream& operator<<(std::ostream& os, StencilOp op);

enum class BlendFactor 
{
	ZERO,
	ONE,
	SRC_COLOR,
	ONE_MINUS_SRC_COLOR,
	DST_COLOR,
	ONE_MINUS_DST_COLOR,
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
	DST_ALPHA,
	ONE_MINUS_DST_ALPHA,
	CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR,
	CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA,
	SRC1_COLOR,
	ONE_MINUS_SRC1_COLOR,
	SRC1_ALPHA,
	ONE_MINUS_SRC1_ALPHA,
};
const std::string BlendFactorToStr(BlendFactor bf);
extern std::ostream& operator<<(std::ostream& os, BlendFactor bf);

enum class BlendOp
{
	ADD,
	SUBTRACT,
	REV_SUBTRACT,
	MIN,
	MAX,
};
const std::string BlendOpToStr(BlendOp op);
extern std::ostream& operator<<(std::ostream& os, BlendOp op);

enum class BorderColor 
{
	DONT_CARE,
	TRANSPARENT_BLACK,
	OPAQUE_BLACK,
	OPAQUE_WHITE,
};
const std::string BorderColorToStr(BorderColor bc);
extern std::ostream& operator<<(std::ostream& os, BorderColor bc);

enum class CullMode
{
	NONE,
	FRONT,
	BACK,
	FRONT_AND_BACK,
};
const std::string CullModeToStr(CullMode mode);
extern std::ostream& operator<<(std::ostream& os, CullMode mode);

enum class Facing
{
	CCW,
	CW,
};
const std::string FacingToStr(Facing face);
extern std::ostream& operator<<(std::ostream& os, Facing face);

enum class Primitive 
{
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	PATCH_LIST,
	LINE_LIST_ADJ,
	LINE_STRIP_ADJ,
	TRIANGLE_LIST_ADJ,
	TRIANGLE_STRIP_ADJ,
	UNDEFINED,
	PRIMITIVE_TYPE_COUNT,
};
const std::string PrimitiveToStr(Primitive primitive);
extern std::ostream& operator<<(std::ostream& os, Primitive primitive);

enum class UniformType
{
	FLOAT1,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	MATRIX4X4,
};
const std::string UniformTypeToStr(UniformType type);
extern std::ostream& operator<<(std::ostream& os, UniformType type);

enum class RPAction
{
	KEEP,
	CLEAR,
	STORE,
	DONT_CARE,
};
const std::string RPActionToStr(RPAction action);
extern std::ostream& operator<<(std::ostream& os, RPAction action);

enum class FBBlitFilter 
{
	NEAREST,
	LINEAR,
};
const std::string FBBlitFilterToStr(FBBlitFilter bf);
extern std::ostream& operator<<(std::ostream& os, FBBlitFilter bf);

enum SamplerFalgs
{
	ARRAY_ON_VULKAN = 0x01,
};

enum BufferUsageFlag
{
	VERTEXDATA = 0x01,
	INDEXDATA  = 0x02,
	GENERIC    = 0x04,
	UNIFORM    = 0x08,
	DYNAMIC    = 0xf0,
};

enum FBChannel 
{
	FB_COLOR_BIT   = 0x01,
	FB_DEPTH_BIT   = 0x02,
	FB_STENCIL_BIT = 0x04,
};

} // namespace Mmp

namespace Mmp
{

/**
 * @param[in]  dataFormat
 * @return     size in byte
 */
size_t DataFormatSizeInBytes(DataFormat dataFormat);

/**
 * @param[in]    u
 * @param[out]   f
 */
void Uint8x4ToFloat4(uint32_t u, float f[4]);

/**
 * @brief 原生数据格式定义
 */
class RawData : public NormalSharedData
{
public:
	using ptr = std::shared_ptr<RawData>;
public:
	explicit RawData(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
};

/**
 * @brief       像素数据格式定义
 */
class GLPixelData : public NormalSharedData
{
public:
	using ptr = std::shared_ptr<GLPixelData>;
public:
	GLPixelData(uint32_t width, uint32_t height, uint32_t depth, DataFormat format, AbstractAllocateMethod::ptr allocateMethod = nullptr);
public:
	uint32_t   width;
	uint32_t   height;
	uint32_t   depth;
	DataFormat format;
};

struct GLRect2D
{
public:
    GLRect2D();
    GLRect2D(int32_t x, int32_t y, int32_t w, int32_t h);
public:
    int32_t    x;
    int32_t    y;
    int32_t    w;
    int32_t    h;
};

struct GLOffset2D
{
public:
    GLOffset2D();
    GLOffset2D(int32_t x, int32_t y);
public:
    int32_t    x;
    int32_t    y;
};

} // namespace Mmp