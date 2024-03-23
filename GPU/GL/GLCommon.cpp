#include "GLCommon.h"

#include <cassert>

namespace Mmp
{

const std::string GPUBackendToStr(GPUBackend backend)
{
    switch (backend)
    {
        case GPUBackend::OPENGL: return "OPENGL";
        case GPUBackend::OPENGL_ES: return "OPENGL_ES";
        case GPUBackend::D3D11: return "D3D11";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, GPUBackend backend)
{
    os << GPUBackendToStr(backend);
    return os;
}

const std::string GpuTaskStatusToStr(GpuTaskStatus status)
{
    switch (status)
    {
        case GpuTaskStatus::NORMAL: return "NORMAL";
        case GpuTaskStatus::PRESENT: return "PRESENT";
        case GpuTaskStatus::EXIT: return "EXIT";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, GpuTaskStatus status)
{
    os << GpuTaskStatusToStr(status);
    return os;
}

const std::string DataFormatToStr(DataFormat format)
{
    switch (format)
    {   
        case DataFormat::UNDEFINED: return "UNDEFINED";

        case DataFormat::R8_UNORM: return "R8_UNORM";
        case DataFormat::R8G8_UNORM: return "R8G8_UNORM";
        case DataFormat::R8G8B8_UNORM: return "R8G8B8_UNORM";
        
        case DataFormat::Y8_UNORM: return "Y8_UNORM";
        case DataFormat::U8_UNORM: return "U8_UNORM";
        case DataFormat::V8_UNORM: return "V8_UNORM";
        case DataFormat::U8V8_UNORM: return "U8V8_UNORM";

        case DataFormat::R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
        case DataFormat::R8G8B8A8_UNORM_SRGB: return "R8G8B8A8_UNORM_SRGB";
        case DataFormat::B8G8R8A8_UNORM: return "B8G8R8A8_UNORM";
        case DataFormat::B8G8R8A8_UNORM_SRGB: return "B8G8R8A8_UNORM_SRGB";

        case DataFormat::NV12_UINT: return "NV12_UINT";
        case DataFormat::R8G8B8A8_SNORM: return "R8G8B8A8_SNORM";
        case DataFormat::R8G8B8A8_UINT: return "R8G8B8A8_UINT";
        case DataFormat::R8G8B8A8_SINT: return "R8G8B8A8_SINT";        

        case DataFormat::R4G4_UNORM_PACK8: return "R4G4_UNORM_PACK8";
        case DataFormat::A4R4G4B4_UNORM_PACK16: return "A4R4G4B4_UNORM_PACK16";
        case DataFormat::B4G4R4A4_UNORM_PACK16: return "B4G4R4A4_UNORM_PACK16";  
        case DataFormat::R4G4B4A4_UNORM_PACK16: return "R4G4B4A4_UNORM_PACK16";
        case DataFormat::R5G6B5_UNORM_PACK16: return "R5G6B5_UNORM_PACK16";
        case DataFormat::B5G6R5_UNORM_PACK16: return "B5G6R5_UNORM_PACK16";  
        case DataFormat::R5G5B5A1_UNORM_PACK16: return "R5G5B5A1_UNORM_PACK16";  
        case DataFormat::B5G5R5A1_UNORM_PACK16: return "B5G5R5A1_UNORM_PACK16";
        case DataFormat::A1R5G5B5_UNORM_PACK16: return "A1R5G5B5_UNORM_PACK16";
        case DataFormat::A1B5G5R5_UNORM_PACK16: return "A1B5G5R5_UNORM_PACK16";  

        case DataFormat::R16_UNORM: return "R16_UNORM"; 

        case DataFormat::R16_FLOAT: return "R16_FLOAT";
        case DataFormat::R16G16_FLOAT: return "R16G16_FLOAT";
        case DataFormat::R16G16B16A16_FLOAT: return "R16G16B16A16_FLOAT";  

        case DataFormat::R32_FLOAT: return "R32_FLOAT";  
        case DataFormat::R32G32_FLOAT: return "R32G32_FLOAT";
        case DataFormat::R32G32B32_FLOAT: return "R32G32B32_FLOAT";
        case DataFormat::R32G32B32A32_FLOAT: return "R32G32B32A32_FLOAT";  

        case DataFormat::BC1_RGBA_UNORM_BLOCK: return "BC1_RGBA_UNORM_BLOCK";  
        case DataFormat::BC1_RGBA_SRGB_BLOCK: return "BC1_RGBA_SRGB_BLOCK";
        case DataFormat::BC2_UNORM_BLOCK: return "BC2_UNORM_BLOCK";
        case DataFormat::BC3_UNORM_BLOCK: return "BC3_UNORM_BLOCK";
        case DataFormat::BC3_SRGB_BLOCK: return "BC3_SRGB_BLOCK";  
        case DataFormat::BC4_UNORM_BLOCK: return "BC4_UNORM_BLOCK";
        case DataFormat::BC4_SNORM_BLOCK: return "BC4_SNORM_BLOCK";
        case DataFormat::BC5_UNORM_BLOCK: return "BC5_UNORM_BLOCK";
        case DataFormat::BC5_SNORM_BLOCK: return "BC5_SNORM_BLOCK";  
        case DataFormat::BC6H_UFLOAT_BLOCK: return "BC6H_UFLOAT_BLOCK";
        case DataFormat::BC6H_SFLOAT_BLOCK: return "BC6H_SFLOAT_BLOCK";
        case DataFormat::BC7_UNORM_BLOCK: return "BC7_UNORM_BLOCK";
        case DataFormat::BC7_SRGB_BLOCK: return "BC7_SRGB_BLOCK";  

        case DataFormat::ETC1: return "ETC1"; 

        case DataFormat::S8: return "S8";
        case DataFormat::D16: return "D16";  
        case DataFormat::D16_S8: return "D16_S8";
        case DataFormat::D24_S8: return "D24_S8";
        case DataFormat::D32F: return "D32F";
        case DataFormat::D32F_S8: return "D32F_S8"; 

        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, DataFormat format)
{
    os << DataFormatToStr(format);
    return os;
}


const std::string TextureFilterToStr(TextureFilter tf)
{
    switch (tf)
    {
        case TextureFilter::NEAREST: return "NEAREST";
        case TextureFilter::LINEAR: return "LINEAR";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, TextureFilter tf)
{
    os << TextureFilterToStr(tf);
    return os;
}

const std::string TextureAddressModeToStr(TextureAddressMode mode)
{
    switch (mode)
    {
        case TextureAddressMode::REPEAT: return "REPEAT";
        case TextureAddressMode::REPEAT_MIRROR: return "REPEAT_MIRROR";
        case TextureAddressMode::CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
        case TextureAddressMode::CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, TextureAddressMode mode)
{
    os << TextureAddressModeToStr(mode);
    return os;
}

const std::string TextureTypeToStr(TextureType type)
{
    switch (type)
    {
        case TextureType::LINEAR1D: return "LINEAR1D";
        case TextureType::LINEAR2D: return "LINEAR2D";
        case TextureType::LINEAR3D: return "LINEAR3D";
        case TextureType::CUBE: return "CUBE";
        case TextureType::ARRAY1D: return "ARRAY1D";
        case TextureType::ARRAY2D: return "ARRAY2D";
        case TextureType::UNKNOWN: return "UNKNOWN";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, TextureType type)
{
    os << TextureTypeToStr(type);
    return os;
}

const std::string ComparisonToStr(Comparison Comparison)
{
    switch (Comparison)
    {
        case Comparison::NEVER: return "NEVER";
        case Comparison::LESS: return "LESS";
        case Comparison::EQUAL: return "EQUAL";
        case Comparison::LESS_EQUAL: return "LESS_EQUAL";
        case Comparison::GREATER: return "GREATER";
        case Comparison::NOT_EQUAL: return "NOT_EQUAL";
        case Comparison::GREATER_EQUAL: return "GREATER_EQUAL";
        case Comparison::ALWAYS: return "ALWAYS";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, Comparison Comparison)
{
    os << ComparisonToStr(Comparison);
    return os;
}

const std::string LogicOpToStr(LogicOp op)
{
    switch (op)
    {
        case LogicOp::CLEAR: return "CLEAR";
        case LogicOp::SET: return "SET";
        case LogicOp::COPY: return "COPY";
        case LogicOp::COPY_INVERTED: return "COPY_INVERTED";
        case LogicOp::NOOP: return "NOOP";
        case LogicOp::INVERT: return "INVERT";
        case LogicOp::AND: return "AND";
        case LogicOp::NAND: return "NAND";
        case LogicOp::OR: return "OR";
        case LogicOp::NOR: return "NOR";
        case LogicOp::XOR: return "XOR";
        case LogicOp::EQUIV: return "EQUIV";
        case LogicOp::AND_REVERSE: return "AND_REVERSE";
        case LogicOp::AND_INVERTED: return "AND_INVERTED";
        case LogicOp::OR_REVERSE: return "OR_REVERSE";
        case LogicOp::OR_INVERTED: return "OR_INVERTED";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, LogicOp op)
{
    os << LogicOpToStr(op);
    return os;
}

const std::string StencilOpToStr(StencilOp op)
{
    switch (op)
    {
        case StencilOp::KEEP: return "KEEP";
        case StencilOp::ZERO: return "ZERO";
        case StencilOp::REPLACE: return "REPLACE";
        case StencilOp::INCREMENT_AND_CLAMP: return "INCREMENT_AND_CLAMP";
        case StencilOp::DECREMENT_AND_CLAMP: return "DECREMENT_AND_CLAMP";
        case StencilOp::INVERT: return "INVERT";
        case StencilOp::INCREMENT_AND_WRAP: return "INCREMENT_AND_WRAP";
        case StencilOp::DECREMENT_AND_WRAP: return "DECREMENT_AND_WRAP";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, StencilOp op)
{
    os << StencilOpToStr(op);
    return os;
}

const std::string BlendFactorToStr(BlendFactor bf)
{
    switch (bf)
    {
        case BlendFactor::ZERO: return "ZERO";
        case BlendFactor::ONE: return "ONE";
        case BlendFactor::SRC_COLOR: return "SRC_COLOR";
        case BlendFactor::ONE_MINUS_SRC_COLOR: return "ONE_MINUS_SRC_COLOR";
        case BlendFactor::DST_COLOR: return "DST_COLOR";
        case BlendFactor::ONE_MINUS_DST_COLOR: return "ONE_MINUS_DST_COLOR";
        case BlendFactor::SRC_ALPHA: return "SRC_ALPHA";
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return "ONE_MINUS_SRC_ALPHA";
        case BlendFactor::DST_ALPHA: return "DST_ALPHA";
        case BlendFactor::ONE_MINUS_DST_ALPHA: return "ONE_MINUS_DST_ALPHA";
        case BlendFactor::CONSTANT_COLOR: return "CONSTANT_COLOR";
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return "ONE_MINUS_CONSTANT_COLOR";
        case BlendFactor::CONSTANT_ALPHA: return "CONSTANT_ALPHA";
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return "ONE_MINUS_CONSTANT_ALPHA";
        case BlendFactor::SRC1_COLOR: return "SRC1_COLOR";
        case BlendFactor::ONE_MINUS_SRC1_COLOR: return "ONE_MINUS_SRC1_COLOR";
        case BlendFactor::SRC1_ALPHA: return "SRC1_ALPHA";
        case BlendFactor::ONE_MINUS_SRC1_ALPHA: return "ONE_MINUS_SRC1_ALPHA";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, BlendFactor bf)
{
    os << BlendFactorToStr(bf);
    return os;
}

const std::string BlendOpToStr(BlendOp op)
{
    switch (op)
    {
        case BlendOp::ADD: return "ADD";
        case BlendOp::SUBTRACT: return "SUBTRACT";
        case BlendOp::REV_SUBTRACT: return "REV_SUBTRACT";
        case BlendOp::MIN: return "MIN";
        case BlendOp::MAX: return "MAX";
        default: assert(false); return "UNKNOWN"; 
    }
}
extern std::ostream& operator<<(std::ostream& os, BlendOp op)
{
    os << BlendOpToStr(op);
    return os;
}

const std::string BorderColorToStr(BorderColor bc)
{
    switch (bc)
    {
        case BorderColor::DONT_CARE: return "DONT_CARE";
        case BorderColor::TRANSPARENT_BLACK: return "TRANSPARENT_BLACK";
        case BorderColor::OPAQUE_BLACK: return "OPAQUE_BLACK";
        case BorderColor::OPAQUE_WHITE: return "OPAQUE_WHITE";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, BorderColor bc)
{
    os << BorderColorToStr(bc);
    return os;
}

const std::string CullModeToStr(CullMode mode)
{
    switch (mode)
    {
        case CullMode::NONE: return "NONE";
        case CullMode::FRONT: return "FRONT";
        case CullMode::BACK: return "BACK";
        case CullMode::FRONT_AND_BACK: return "FRONT_AND_BACK";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, CullMode mode)
{
    os << CullModeToStr(mode);
    return os;
}

const std::string FacingToStr(Facing face)
{
    switch (face)
    {
        case Facing::CCW: return "CCW";
        case Facing::CW:  return "CW";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, Facing face)
{
    os << FacingToStr(face);
    return os;
}


const std::string PrimitiveToStr(Primitive primitive)
{
    switch (primitive)
    {
        case Primitive::POINT_LIST: return "POINT_LIST";
        case Primitive::LINE_LIST: return "LINE_LIST";
        case Primitive::LINE_STRIP: return "LINE_STRIP";
        case Primitive::TRIANGLE_LIST: return "TRIANGLE_LIST";
        case Primitive::TRIANGLE_STRIP: return "TRIANGLE_STRIP";
        case Primitive::TRIANGLE_FAN: return "TRIANGLE_FAN";
        // Tesselation shader only
        case Primitive::PATCH_LIST: return "PATCH_LIST";
        // These are for geometry shaders only.
        case Primitive::LINE_LIST_ADJ: return "LINE_LIST_ADJ";
        case Primitive::LINE_STRIP_ADJ: return "LINE_STRIP_ADJ";
        case Primitive::TRIANGLE_LIST_ADJ: return "TRIANGLE_LIST_ADJ";
        case Primitive::TRIANGLE_STRIP_ADJ: return "TRIANGLE_STRIP_ADJ";
        case Primitive::UNDEFINED: return "UNDEFINED";
        case Primitive::PRIMITIVE_TYPE_COUNT: return "PRIMITIVE_TYPE_COUNT";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, Primitive primitive)
{
    os << PrimitiveToStr(primitive);
    return os;
}

const std::string UniformTypeToStr(UniformType type)
{
    switch (type)
    {
        case UniformType::FLOAT1: return "FLOAT1";
        case UniformType::FLOAT2: return "FLOAT2";
        case UniformType::FLOAT3: return "FLOAT3";
        case UniformType::FLOAT4: return "FLOAT4";
        case UniformType::MATRIX4X4: return "MATRIX4X4";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, UniformType type)
{
    os << UniformTypeToStr(type);
    return os;
}

const std::string RPActionToStr(RPAction action)
{
    switch (action)
    {
        case RPAction::KEEP: return "KEEP";
        case RPAction::CLEAR: return "CLEAR";
        case RPAction::STORE: return "STORE";
        case RPAction::DONT_CARE: return "DONT_CARE";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, RPAction action)
{
    os << RPActionToStr(action);
    return os;
}

const std::string FBBlitFilterToStr(FBBlitFilter bf)
{
    switch (bf)
    {
        case FBBlitFilter::NEAREST: return "NEAREST";
        case FBBlitFilter::LINEAR: return "LINEAR";
        default: assert(false); return "UNKNOWN"; 
    }
}

extern std::ostream& operator<<(std::ostream& os, FBBlitFilter bf)
{
    os << FBBlitFilterToStr(bf);
    return os;
}

} // namespace Mmp

namespace Mmp
{

size_t DataFormatSizeInBytes(DataFormat dataFormat)
{
	switch (dataFormat) 
    {
        case DataFormat::R8_UNORM: return 1;
        case DataFormat::R8G8_UNORM: return 2;
        case DataFormat::R8G8B8_UNORM: return 3;

        case DataFormat::Y8_UNORM:   return 1;
        case DataFormat::U8_UNORM:   return 1;
        case DataFormat::V8_UNORM:   return 1;
        case DataFormat::U8V8_UNORM: return 2;

        case DataFormat::R4G4_UNORM_PACK8: return 1;
        case DataFormat::R4G4B4A4_UNORM_PACK16: return 2;
        case DataFormat::B4G4R4A4_UNORM_PACK16: return 2;
        case DataFormat::A4R4G4B4_UNORM_PACK16: return 2;
        case DataFormat::R5G5B5A1_UNORM_PACK16: return 2;
        case DataFormat::B5G5R5A1_UNORM_PACK16: return 2;
        case DataFormat::R5G6B5_UNORM_PACK16: return 2;
        case DataFormat::B5G6R5_UNORM_PACK16: return 2;
        case DataFormat::A1R5G5B5_UNORM_PACK16: return 2;

        case DataFormat::R8G8B8A8_UNORM:
        case DataFormat::R8G8B8A8_UNORM_SRGB: return 4;
        case DataFormat::B8G8R8A8_UNORM:
        case DataFormat::B8G8R8A8_UNORM_SRGB: return 4;

        case DataFormat::NV12_UINT: return 2;
        case DataFormat::R8G8B8A8_SNORM: return 4;
        case DataFormat::R8G8B8A8_UINT: return 4;
        case DataFormat::R8G8B8A8_SINT: return 4;

        case DataFormat::R16_UNORM: return 2;

        case DataFormat::R16_FLOAT: return 2;
        case DataFormat::R16G16_FLOAT: return 4;
        case DataFormat::R16G16B16A16_FLOAT: return 8;
        case DataFormat::R32_FLOAT: return 4;
        case DataFormat::R32G32_FLOAT: return 8;
        case DataFormat::R32G32B32_FLOAT: return 12;
        case DataFormat::R32G32B32A32_FLOAT: return 16;

        case DataFormat::S8: return 1;
        case DataFormat::D16: return 2;
        case DataFormat::D16_S8: return 3;
        case DataFormat::D24_S8: return 4;
        case DataFormat::D32F: return 4;
        case DataFormat::D32F_S8: return 5;

        default: assert(false); return 0;
	}
}

void Uint8x4ToFloat4(uint32_t u, float f[4])
{
    f[0] = (((u >> 0)) & 0xFF) * (1.0f / 255.0f);
    f[1] = (((u >> 8)) & 0xFF) * (1.0f / 255.0f);
    f[2] = (((u >> 16)) & 0xFF) * (1.0f / 255.0f);
    f[3] = (((u >> 24)) & 0xFF) * (1.0f / 255.0f);
}

RawData::RawData(size_t size, AbstractAllocateMethod::ptr allocateMethod)
    : NormalSharedData(size, allocateMethod)
{
}

GLPixelData::GLPixelData(uint32_t width, uint32_t height, uint32_t depth, DataFormat format, AbstractAllocateMethod::ptr allocateMethod)
    : NormalSharedData(0, allocateMethod)
{
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->format = format;
    SetCapacity(width * height * depth * DataFormatSizeInBytes(format));
    SetSize(width * height * depth * DataFormatSizeInBytes(format));
}

GLRect2D::GLRect2D()
{
    x = -1;
    y = -1;
    w = -1;
    h = -1;
} 

GLRect2D::GLRect2D(int32_t x, int32_t y, int32_t w, int32_t h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

GLOffset2D::GLOffset2D()
{
    x = -1;
    y = -1;
}

GLOffset2D::GLOffset2D(int32_t x, int32_t y)
{
    this->x = x;
    this->y = y;
}

} // namespace Mmp