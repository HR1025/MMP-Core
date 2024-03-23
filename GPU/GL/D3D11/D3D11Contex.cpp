#include "D3D11Contex.h"

#include <cassert>
#include <memory.h>

namespace Mmp
{

bool operator<(const D3D11DepthStencilKey& left, const D3D11DepthStencilKey& right)
{
    if (left.desc < right.desc)
    {
        return true;
    }
    else if (left.compareMask < right.compareMask)
    {
        return true;
    }
    else if (left.writeMask < right.writeMask)
    {
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace Mmp

namespace Mmp
{

D3D11ShaderModule::D3D11ShaderModule(SL::ShaderStage stage, const std::string& tag)
{
    _stage = stage;
    _tag   = tag;
    
    vs     = nullptr;
    ps     = nullptr;
    gs     = nullptr;
}

SL::ShaderStage D3D11ShaderModule::GetStage()
{
    return _stage;
}

D3D11ShaderModule::~D3D11ShaderModule()
{
    if (vs)
    {
        vs->Release();
    }
    if (ps)
    {
        ps->Release();
    }
    if (gs)
    {
        gs->Release();
    }
}

D3D11BlendState::D3D11BlendState()
{
    bs = nullptr;
    memset(blendFactor, 0, sizeof(blendFactor));
}

D3D11BlendState::~D3D11BlendState()
{
    if (bs)
    {
        bs->Release();
    }
}

D3D11SamplerState::D3D11SamplerState()
{
    ss = nullptr;
}

D3D11SamplerState::~D3D11SamplerState()
{
    if (ss)
    {
        ss->Release();
    }
}

D3D11RasterState::D3D11RasterState()
{
    rs = nullptr;
}

D3D11RasterState::~D3D11RasterState()
{
    if (rs)
    {
        rs->Release();
    }
}

D3D11Pipeline::D3D11Pipeline()
{
    vs = nullptr;
    ps = nullptr;
    gs = nullptr;
    il = nullptr;

    topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    
    dynamicUniformsSize = 0;
    dynamicUniforms = nullptr;
}

D3D11Pipeline::~D3D11Pipeline()
{
    if (dynamicUniforms)
    {
        dynamicUniforms->Release();
    }
    if (il)
    {
        il->Release();
    }
}

D3D11Texture::D3D11Texture(const TextureDesc& desc)
{
    _width   = desc.width;
    _height  = desc.height;
    _depth   = desc.depth;
    mipLevel = desc.mipLevels;
    format   = desc.format;

    tex = nullptr;
    stagingTex = nullptr;
    packingTex = nullptr;
    view = nullptr;
    
    colorRTView = nullptr;
    colorSRView = nullptr;
}

D3D11Texture::~D3D11Texture()
{
    if (tex)
    {
        tex->Release();
    }
    if (stagingTex)
    {
        stagingTex->Release();
    }
    if (view)
    {
        view->Release();
    }
    if (colorRTView)
    {
        colorRTView->Release();
    }
    if (colorSRView)
    {
        colorSRView->Release();
    }
}

bool D3D11Texture::Create(ID3D11DeviceContext* context, ID3D11Device* device, const TextureDesc& desc, bool generateMips)
{
    UINT BindFlags = D3D11_BIND_SHADER_RESOURCE;
    UINT MiscFlags = 0;
    HRESULT hr = S_OK;
    
    if (generateMips || desc.miscFlag & GlTextureFlags::TEXTURE_USE_FOR_RENDER)
    {
        BindFlags |= D3D11_BIND_RENDER_TARGET;
    }
    if (generateMips)
    {
        MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }

    D3D11_TEXTURE2D_DESC colorDesc = {};
    colorDesc.Width = desc.width;
    colorDesc.Height = desc.height;
    colorDesc.MipLevels = desc.mipLevels;
    colorDesc.ArraySize = 1;
    colorDesc.Format = DataFormatToD3D11(desc.format);
    colorDesc.SampleDesc.Count = 1;
    colorDesc.SampleDesc.Quality = 0;
    colorDesc.Usage = D3D11_USAGE_DEFAULT;
    colorDesc.BindFlags = BindFlags;
    colorDesc.MiscFlags = MiscFlags;
    colorDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA* initDataParam = nullptr;
    D3D11_SUBRESOURCE_DATA initData[12];  // 4K, 4096 = 12 power of 2
    std::vector<uint8_t> initDataBuffer[12];
    if (desc.initData.size() && !generateMips)
    {
        int w = desc.width;
        int h = desc.height;
        int d = desc.depth;
        for (size_t i=0; i < desc.initData.size(); i++)
        {
            uint32_t byteStride = (uint32_t)DataFormatSizeInBytes(desc.format) * (uint32_t)w;
            initData[i].pSysMem = desc.initData[i]->GetData();
            initData[i].SysMemPitch = (UINT)byteStride;
            initData[i].SysMemSlicePitch = (UINT)(h * byteStride);
            w = (w + 1) / 2;
            h = (h + 1) / 2;
            d = (d + 1) / 2;
        }
        initDataParam = initData;
    }
    if (FAILED(device->CreateTexture2D(&colorDesc, initDataParam, &tex)))
    {
        assert(false);
        tex = nullptr;
        return false;
    }
    if (FAILED(device->CreateShaderResourceView(tex, nullptr, &view)))
    {
        assert(false);
        view = nullptr;
        return false;
    }

    if (generateMips && desc.initData.size() >= 1)
    {
        uint32_t byteStride = desc.width * (uint32_t)DataFormatSizeInBytes(desc.format);
        // TO CHECK : continuous memory layout
        context->UpdateSubresource(tex, 0, nullptr, desc.initData[0]->GetData(), byteStride, 0);
        context->GenerateMips(view);
    }
    else
    {
        // TO CHECK
    }

    if (desc.miscFlag & GlTextureFlags::TEXTURE_USE_FOR_RENDER)
    {
        hr = device->CreateRenderTargetView(tex, nullptr, &colorRTView);
        if (FAILED(hr))
        {
            GL_LOG_ERROR << "ID3D11Device::CreateRenderTargetView fail, error is:" << D3D11ReturnCodeToStr(hr);
            assert(false);
            return false;
        }
        hr = device->CreateShaderResourceView(tex, nullptr, &colorSRView);
        if (FAILED(hr))
        {
            GL_LOG_ERROR << "ID3D11Device::CreateRenderTargetView fail, error is:" << D3D11ReturnCodeToStr(hr);
            assert(false);
            return false;
        }
    }

    return true;
}

void D3D11Texture::Update(ID3D11DeviceContext* context, ID3D11Device* device, const TextureDesc& desc)
{
    if (!CreateStagingTexture(device))
    {
        assert(false);
        return;
    }
    uint32_t width = desc.width;
    uint32_t height = desc.height;
    uint32_t depth = desc.depth;
    for (auto& data : desc.initData)
    {
        FillLevel(context, 0 /* to check */, width, height, depth, (const uint8_t*)data->GetData());
    }
    context->CopyResource(tex, stagingTex);
}

bool D3D11Texture::ReadBack(ID3D11DeviceContext* context, ID3D11Device* device, int x, int y, int w, int h, uint8_t* pixel)
{
    if (!CreatePackingTexture(device))
    {
        assert(false);
        return false;
    }
    D3D11_BOX srcBox = {};
    {
        srcBox.left = 0;
        srcBox.right = _width;
        srcBox.top = 0;
        srcBox.bottom = _height;
        srcBox.front = 0;
        srcBox.back = 1;  
    }
    context->CopySubresourceRegion(packingTex, 0, 0, 0, 0, tex, 0, &srcBox);
    D3D11_MAPPED_SUBRESOURCE map = {};
    if (FAILED(context->Map(packingTex, 0, D3D11_MAP_READ, 0, &map)))
    {
        assert(false);
        return false;
    }
    uint32_t align = (uint32_t)DxgiFormatSize(DataFormatToD3D11(format));
    // Workaround : D3D11 不支持 RGB888 类型
    //              故 D3D11 内部分配为 ARGB 类型,在回读时统一进行拷贝
    if (format == DataFormat::R8G8B8_UNORM)
    {
        for (int i=0; i<h; i++)
        {
            for (int j=0; j<w; j++)
            {
                const uint32_t* src = (uint32_t*)((uint8_t*)map.pData + map.RowPitch * (y+i) + 4 * (x+j));
                uint8_t* dst = pixel + i*w*3 + j*3;
                dst[0] = (*src & 0x00FF0000) >> 16;
                dst[1] = (*src & 0x0000FF00) >> 8;
                dst[2] = (*src & 0x000000FF);
            }
        }
    }
    else if (x == 0 && y == 0 && w == _width && h == _height 
             // Hint : 根据实际测试, RowPitch 似乎不一定等于 _width * align, D3D11 可能最小对齐长度为 1024 
            && map.RowPitch == _width * align) 
    {
        memcpy(pixel, map.pData, _width * _height * align);        
    }
    else
    {
        uint8_t* src = (uint8_t*)map.pData;
        uint8_t* dst = (uint8_t*)pixel;
        for (int i=0; i<h; i++)
        {
            memcpy(dst + i*w*align, src + map.RowPitch * (y+i) + x*align, w*align);
        }
    }
    context->Unmap(packingTex, 0);
    return true;
}

bool D3D11Texture::CreatePackingTexture(ID3D11Device* device)
{
    if (packingTex)
    {
        return true;
    }
    D3D11_TEXTURE2D_DESC colorDesc = {};
    colorDesc.Width = _width;
    colorDesc.Height = _height;
    colorDesc.MipLevels = mipLevel;
    colorDesc.ArraySize = 1;
    colorDesc.Format = DataFormatToD3D11(format);
    colorDesc.SampleDesc.Count = 1;
    colorDesc.SampleDesc.Quality = 0;
    colorDesc.Usage = D3D11_USAGE_STAGING;
    colorDesc.BindFlags = 0;
    colorDesc.MiscFlags = 0;
    colorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    if (SUCCEEDED(device->CreateTexture2D(&colorDesc, nullptr, &packingTex)))
    {
        return true;
    }
    else
    {
        packingTex = nullptr;
        GL_LOG_ERROR << "ID3D11Device::CreateTexture2D Fail";
        return false;
    }
}

bool D3D11Texture::CreateStagingTexture(ID3D11Device* device)
{
    if (stagingTex)
    {
        return true;
    }
    D3D11_TEXTURE2D_DESC colorDesc = {};
    colorDesc.Width = _width;
    colorDesc.Height = _height;
    colorDesc.MipLevels = mipLevel;
    colorDesc.ArraySize = 1;
    colorDesc.Format = DataFormatToD3D11(format);
    colorDesc.SampleDesc.Count = 1;
    colorDesc.SampleDesc.Quality = 0;
    colorDesc.Usage = D3D11_USAGE_STAGING;
    colorDesc.BindFlags = 0;
    colorDesc.MiscFlags = 0;
    colorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (SUCCEEDED(device->CreateTexture2D(&colorDesc, nullptr, &stagingTex)))
    {
        return true;
    }
    else
    {
        stagingTex = nullptr;
        GL_LOG_ERROR << "ID3D11Device::CreateTexture2D Fail";
        return false;
    }
}

bool D3D11Texture::FillLevel(ID3D11DeviceContext* context, int level, int w, int h, int d, const uint8_t* data)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(context->Map(stagingTex, level, D3D11_MAP_WRITE, 0, &mapped)))
    {
        tex->Release();
        tex = nullptr;
        return false;
    }

    if (format != DataFormat::R8G8B8_UNORM)
    {
        // Hint : 根据实际测试, D3D11 内部可能存在字节对齐的行为, 即输入为 1080 下, RowPitch 打印实际为 2048,
        //        故逐行拷贝能够保证数据正确更新
        // Todo : a better way
        for (int s=0; s<d; s++)
        {
            for (int y=0; y<h; y++)
            {
                void* dest = (uint8_t*)mapped.pData + mapped.DepthPitch * s + mapped.RowPitch * y;
                uint32_t byteStride = w * (uint32_t)DataFormatSizeInBytes(format);
                const uint8_t* src = data + byteStride * (y + h * s);
                memcpy(dest, src, byteStride);
            }
        }
    }
    // Workaround : D3D11 不支持 RGB888 类型
    //              故 D3D11 内部分配为 ARGB 类型,在更新纹理时逐一进行拷贝
    else
    {
        for (int s=0; s<d; s++)
        {
            for (int y=0; y<h; y++)
            {
                for (int z=0; z<w; z++)
                {
                    uint32_t* dest = (uint32_t*)((uint8_t*)mapped.pData + mapped.DepthPitch * s + mapped.RowPitch * y + 4 * z);
                    const uint8_t* src = data + 3 * w * (y + h * s) + z * 3;
                    *dest = (0xFF << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
                }
            }
        }
    }
    context->Unmap(stagingTex, level);
    return true;
}

D3D11Buffer::D3D11Buffer()
{
    buf = nullptr;
    srView = nullptr;
    size = 0;
}

D3D11Buffer::~D3D11Buffer()
{
    if (buf)
    {
        buf->Release();
    }
    if (srView)
    {
        srView->Release();
    }
}

D3D11FrameBuffer::D3D11FrameBuffer(int width, int height, std::vector<D3D11Texture::ptr> colorTexs)
{
    _width = width;
    _height = height;

    this->colorTexs = colorTexs;
    depthSRView = nullptr;

    depthStencilTex = nullptr;
    depthStencilRTView = nullptr;
    depthStencilFormat = DXGI_FORMAT_UNKNOWN;
}

D3D11FrameBuffer::~D3D11FrameBuffer()
{
    if (depthStencilTex)
        depthStencilTex->Release();
    if (depthStencilRTView)
        depthStencilRTView->Release();
}

bool D3D11FrameBuffer::CreateDefaultColorTex(ID3D11DeviceContext* context, ID3D11Device* device)
{
    TextureDesc desc;
    desc.width = _width;
    desc.height = _height;
    desc.depth = 1;
    desc.format = DataFormat::R8G8B8A8_UNORM;
    desc.type = TextureType::LINEAR2D;
    desc.mipLevels = 1;
    desc.miscFlag = GlTextureFlags::TEXTURE_USE_FOR_RENDER;
    desc.generateMips = false;
    colorTexs[0] = std::make_shared<D3D11Texture>(desc);
    return colorTexs[0]->Create(context, device, desc, desc.generateMips);
}

bool D3D11FrameBuffer::Create(ID3D11DeviceContext* context, ID3D11Device* device, bool zStencil)
{
    bool res = true;
    if (colorTexs.empty())
    {
        colorTexs.resize(1);
        res = CreateDefaultColorTex(context, device);
    }
    if (zStencil)
    {
        /* res = */ CreateDefaultDepthStencilTex(context, device);
    }
    return res;
}

bool D3D11FrameBuffer::CreateDefaultDepthStencilTex(ID3D11DeviceContext* context, ID3D11Device* device)
{
    HRESULT hr = S_OK;
    depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = _width;
    depthDesc.Height = _height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;
    hr = device->CreateTexture2D(&depthDesc, nullptr, &depthStencilTex);
    if (FAILED(hr))
    {
        GL_LOG_ERROR << "ID3D11Device::CreateTexture2D fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return false;
    }
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture1D.MipSlice = 0;
    hr = device->CreateDepthStencilView(depthStencilTex, &dsvDesc, &depthStencilRTView);
    if (FAILED(hr))
    {
        GL_LOG_ERROR << "ID3D11Device::CreateDepthStencilView fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return false;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC depthViewDesc = {};
    depthViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    depthViewDesc.Texture2D.MostDetailedMip = 0;
    depthViewDesc.Texture2D.MipLevels = 1;
    depthViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    hr = device->CreateShaderResourceView(depthStencilTex, &depthViewDesc, &depthSRView);
    if (FAILED(hr))
    {
        GL_LOG_WARN << "ID3D11Device::CreateShaderResourceView fail, error is:" << D3D11ReturnCodeToStr(hr);
        depthSRView = nullptr;
        return false;
    }
    return true;
}

} // namespace Mmp