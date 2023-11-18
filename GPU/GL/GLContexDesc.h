//
// GLContexDesc.h
//
// Library: GPU
// Package: GL
// Module:  GL
// 

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "GLCommon.h"
#include "GLContex.h"

namespace Mmp
{

struct StencilSetup
{
public:
	StencilSetup();
public:
	StencilOp failOp;
	StencilOp passOp;
	StencilOp depthFailOp;
	Comparison compareOp;
};

struct SamplerDef
{
public:
	SamplerDef();
	SamplerDef(const std::string& name, SamplerFalgs falgs = SamplerFalgs::ARRAY_ON_VULKAN);
public:
	std::string   name;
	SamplerFalgs  falgs;
};

struct Viewport 
{
	float TopLeftX;
	float TopLeftY;
	float Width;
	float Height;
	float MinDepth;
	float MaxDepth;
};

} // namespace Mmp

namespace Mmp
{

struct BindingDesc 
{
	int stride;
	bool instanceRate;
};

struct AttributeDesc 
{
public:
	AttributeDesc();
	AttributeDesc(const std::string& attribute, int binding, int location, DataFormat format, int offset);
public:
	std::string attribute;
	int binding;
	int location;  // corresponds to semantic
	DataFormat format;
	int offset;
};

struct UniformDesc
{
public:
	UniformDesc();
	UniformDesc(const std::string& name, UniformType type, int16_t offset);
public:
	std::string name;
	UniformType type;
	int16_t     offset;
};

struct UniformBufferDesc
{
public:
	UniformBufferDesc();
public:
	size_t uniformBufferSize;
	std::vector<UniformDesc> uniforms;
};

struct DepthStencilStateDesc 
{
public:
	DepthStencilStateDesc();
public:
	bool depthTestEnabled;
	bool depthWriteEnabled;
	Comparison depthCompare;
	bool stencilEnabled;
	StencilSetup stencil;
}; 
bool operator<(const DepthStencilStateDesc& left, const DepthStencilStateDesc& right);

struct TextureDesc
{
public:
	TextureDesc();
public:
	TextureType type;
	DataFormat  format;

	uint32_t         width;
	uint32_t         height;
	uint32_t         depth;
	int              mipLevels;
	bool             generateMips;
	TextureMiscFlag  miscFlag;

	std::string tag;
	std::vector<GLPixelData::ptr> initData; // Hint : order by level
};

struct FrameBufferDesc
{
public:
	FrameBufferDesc();
public:
	int width;
	int height;
	int depth;
	int numLayers;
	int multiSampleLevel;
	int zStencil;
	std::vector<Texture::ptr> colorTexs;
	std::string tag;
};

struct BlendStateDesc 
{
public:
	BlendStateDesc();
public:
	bool enabled;
	int colorMask;
	BlendFactor srcCol;
	BlendFactor dstCol;
	BlendOp eqCol;
	BlendFactor srcAlpha;
	BlendFactor dstAlpha;
	BlendOp eqAlpha;
	bool logicEnabled;
	LogicOp logicOp;
};

struct SamplerStateDesc 
{
public:
	SamplerStateDesc();
public:
	TextureFilter magFilter;
	TextureFilter minFilter;
	TextureFilter mipFilter;
	float maxAniso;
	TextureAddressMode wrapU;
	TextureAddressMode wrapV;
	TextureAddressMode wrapW;
	bool shadowCompareEnabled;
	Comparison shadowCompareFunc;
	BorderColor borderColor;
};

struct RasterStateDesc
{
public:
	RasterStateDesc();
public:
	CullMode cull;
	Facing frontFace;	
};

struct PipelineDesc
{
	Primitive prim;
	std::vector<ShaderModule::ptr> shaders;
	InputLayout::ptr inputLayout;
	DepthStencilState::ptr depthStencil;
	BlendState::ptr blend;
	RasterState::ptr raster;
	UniformBufferDesc uniformBufferDesc;
	std::vector<SamplerDef> samplers;
};

struct InputLayoutDesc 
{
	std::vector<BindingDesc> bindings;
	std::vector<AttributeDesc> attributes;
};

struct RenderPassInfo
{
public:
	RenderPassInfo();
public:
	RPAction color;
	RPAction depth;
	RPAction stencil;
	uint32_t clearColor;
	float clearDepth;
	uint8_t clearStencil;
};

} // namespace Mmp