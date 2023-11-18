//
// D3D11Contex.h
//
// Library: GPU
// Package: GL
// Module:  D3D11
// 

#pragma once


#include <vector>
#include <memory>
#include <functional>

#include "D3D11Common.h"

namespace Mmp
{

struct D3D11DepthStencilKey
{
    DepthStencilStateDesc desc;
    uint8_t               writeMask;
    uint8_t               compareMask;
};
bool operator<(const D3D11DepthStencilKey& left, const D3D11DepthStencilKey& right);

} // namespace Mmp

namespace Mmp
{

class D3D11ShaderModule : public ShaderModule
{
public:
    using ptr = std::shared_ptr<D3D11ShaderModule>;
public:
    D3D11ShaderModule(SL::ShaderStage stage, const std::string& tag);
    ~D3D11ShaderModule();
public:
    SL::ShaderStage GetStage() override;
public:
    std::vector<uint8_t>       byteCode;
    ID3D11VertexShader*        vs;
    ID3D11PixelShader*         ps;
    ID3D11GeometryShader*      gs;
private:
	SL::ShaderStage            _stage;
    std::string                _tag;
};

class D3D11BlendState : public BlendState
{
public:
    using ptr = std::shared_ptr<D3D11BlendState>;
public:
    D3D11BlendState();
    ~D3D11BlendState();
public:
    ID3D11BlendState*  bs;
    float              blendFactor[4];
};

class D3D11SamplerState : public SamplerState 
{
public:
	using ptr = std::shared_ptr<D3D11SamplerState>;
public:
    D3D11SamplerState();
    ~D3D11SamplerState();
public:
    ID3D11SamplerState* ss;
};

class D3D11RasterState : public RasterState
{
public:
	using ptr = std::shared_ptr<D3D11RasterState>;
public:
    D3D11RasterState();
    ~D3D11RasterState();
public:
    ID3D11RasterizerState* rs;
};

class D3D11InputLayout : public InputLayout
{
public:
	using ptr = std::shared_ptr<D3D11InputLayout>;
public:
    InputLayoutDesc                         desc;
    std::vector<D3D11_INPUT_ELEMENT_DESC>   elements;
    std::vector<int>                        strides;
};

class D3D11DepthStencilState : public DepthStencilState 
{
public:
	using ptr = std::shared_ptr<D3D11DepthStencilState>;
public:
    DepthStencilStateDesc desc;
};

class D3D11Pipeline : public Pipeline
{
public:
    using ptr = std::shared_ptr<D3D11Pipeline>;
public:
    D3D11Pipeline();
    ~D3D11Pipeline();
public:
    D3D11InputLayout::ptr         input;
    D3D11BlendState::ptr          blend;
    D3D11RasterState::ptr         raster;
    D3D11DepthStencilState::ptr   depthStencil;
public:
    ID3D11VertexShader*           vs;
    ID3D11PixelShader*            ps;
    ID3D11GeometryShader*         gs;
    ID3D11InputLayout*            il;
public:
    D3D11_PRIMITIVE_TOPOLOGY             topology;
    std::vector<D3D11ShaderModule::ptr>  shaderModules;
    size_t                               dynamicUniformsSize;
    ID3D11Buffer*                        dynamicUniforms;
};

class D3D11Texture : public Texture
{
public:
    using ptr = std::shared_ptr<D3D11Texture>;
public:
    explicit D3D11Texture(const TextureDesc& desc);
    ~D3D11Texture();
public:
    bool Create(ID3D11DeviceContext* context, ID3D11Device* device, const TextureDesc& desc, bool generateMips);
    void Update(ID3D11DeviceContext* context, ID3D11Device* device, const TextureDesc& desc);
    bool ReadBack(ID3D11DeviceContext* context, ID3D11Device* device, int x, int y, int w, int h, uint8_t* pixel);
public:
    ID3D11Texture2D*            tex;
    ID3D11Texture2D*            stagingTex;
    ID3D11Texture2D*            packingTex;
    ID3D11ShaderResourceView*   view;
    uint32_t                    mipLevel;
    DataFormat                  format;
public:
    ID3D11RenderTargetView*     colorRTView;
    ID3D11ShaderResourceView*   colorSRView;
private:
    bool CreatePackingTexture(ID3D11Device* device);
    bool CreateStagingTexture(ID3D11Device* device);
    bool FillLevel(ID3D11DeviceContext* context, int level, int w, int h, int d, const uint8_t* data);
};

class D3D11Buffer : public GLBuffer
{
public:
    using ptr = std::shared_ptr<D3D11Buffer>;
public:
    D3D11Buffer();
    ~D3D11Buffer();
public:
    ID3D11Buffer*             buf;
    ID3D11ShaderResourceView* srView;
    size_t                    size;
};

class D3D11FrameBuffer : public FrameBuffer
{
public:
    using ptr = std::shared_ptr<D3D11FrameBuffer>;
public:
    D3D11FrameBuffer(int width, int height, std::vector<D3D11Texture::ptr> colorTexs = std::vector<D3D11Texture::ptr>());
    ~D3D11FrameBuffer();
public:
    bool Create(ID3D11DeviceContext* context, ID3D11Device* device, bool zStencil);
public:
    std::vector<D3D11Texture::ptr> colorTexs;
public:
    ID3D11ShaderResourceView* depthSRView;
    ID3D11Texture2D*          depthStencilTex;
    ID3D11DepthStencilView*   depthStencilRTView;
    DXGI_FORMAT               depthStencilFormat;
private:
    bool CreateDefaultColorTex(ID3D11DeviceContext* context, ID3D11Device* device);
    bool CreateDefaultDepthStencilTex(ID3D11DeviceContext* context, ID3D11Device* device);
};

} // namespace Mmp