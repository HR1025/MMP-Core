//
// GLContex.h
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

namespace Mmp
{

class ShaderModule
{
public:
	using ptr = std::shared_ptr<ShaderModule>;
public:
	virtual ~ShaderModule() = default;
	virtual SL::ShaderStage GetStage() = 0;
};

class BlendState
{
public:
	using ptr = std::shared_ptr<BlendState>;
public:
	virtual ~BlendState() = default;
};

class SamplerState
{
public:
	using ptr = std::shared_ptr<SamplerState>;
public:
	virtual ~SamplerState() = default;
};

class DepthStencilState
{
public:
	using ptr = std::shared_ptr<DepthStencilState>;
public:
	virtual ~DepthStencilState() = default;
};

class RasterState
{
public:
	using ptr = std::shared_ptr<RasterState>;
public:
	virtual ~RasterState() = default;
};

class InputLayout
{
public:
	using ptr = std::shared_ptr<InputLayout>;
public:
	virtual ~InputLayout() = default;
};


class Pipeline
{
public:
	using ptr = std::shared_ptr<Pipeline>;
public:
	virtual ~Pipeline() = default;
};

class FrameBuffer
{
public:
	using ptr = std::shared_ptr<FrameBuffer>;
public:
	virtual ~FrameBuffer() = default;
public:
	int Width();
	int Height();
	int Layers();
	int MultiSampleLevel();
protected:
	int _width = -1;
	int _height = -1;
	int _layers = -1;
	int _multiSampleLevel = -1;
};

/**
 * @note MMP 只用 mmp 一级命名空间, Buffer 过于常见,故追加 GL 前缀
 * @todo 其他方案,如多级命令空间 
 */
class GLBuffer
{
public:
	using ptr = std::shared_ptr<GLBuffer>;
public:
	virtual ~GLBuffer() = default;
};

class Texture
{
public:
	using ptr = std::shared_ptr<Texture>;
public:
	virtual ~Texture() = default;
public:
	int Width();
	int Height();
	int Depth();
protected:
	int _width;
	int _height;
	int _depth;
};

} // namespace Mmp