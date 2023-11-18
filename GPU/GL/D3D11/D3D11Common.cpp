#include "D3D11Common.h"

namespace Mmp
{

DXGI_FORMAT DataFormatToD3D11(DataFormat format)
{
    switch (format)
    {
        case DataFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case DataFormat::R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case DataFormat::Y8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case DataFormat::U8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case DataFormat::V8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case DataFormat::U8V8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case DataFormat::R8G8B8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM; // Bad solution
        case DataFormat::R5G6B5_UNORM_PACK16: return DXGI_FORMAT_B5G6R5_UNORM;
        case DataFormat::B5G6R5_UNORM_PACK16: return DXGI_FORMAT_B5G6R5_UNORM;
        case DataFormat::R5G5B5A1_UNORM_PACK16: return DXGI_FORMAT_B5G5R5A1_UNORM;
        case DataFormat::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case DataFormat::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
        case DataFormat::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
        case DataFormat::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DataFormat::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DataFormat::R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DataFormat::B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DataFormat::B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case DataFormat::R16_UNORM: return DXGI_FORMAT_R16_UNORM;
        case DataFormat::R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
        case DataFormat::R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
        case DataFormat::R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case DataFormat::D24_S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case DataFormat::D16: return DXGI_FORMAT_D16_UNORM;
        case DataFormat::D32F: return DXGI_FORMAT_D32_FLOAT;
        case DataFormat::D32F_S8: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case DataFormat::BC1_RGBA_UNORM_BLOCK: return DXGI_FORMAT_BC2_UNORM;
        case DataFormat::BC2_UNORM_BLOCK: return DXGI_FORMAT_BC2_UNORM;
        case DataFormat::BC3_UNORM_BLOCK: return DXGI_FORMAT_BC3_UNORM;
        case DataFormat::BC4_UNORM_BLOCK: return DXGI_FORMAT_BC4_UNORM;
        case DataFormat::BC5_UNORM_BLOCK: return DXGI_FORMAT_BC5_UNORM;
        case DataFormat::BC7_UNORM_BLOCK: return DXGI_FORMAT_BC7_UNORM;
        default:
            assert(false);
            return DXGI_FORMAT_UNKNOWN;
    }
}

size_t DxgiFormatSize(DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_R8_UNORM: return 1;
        case DXGI_FORMAT_R8G8_UNORM: return 2;
        case DXGI_FORMAT_R32_FLOAT: return 4;
        case DXGI_FORMAT_R32G32_FLOAT: return 8;
        case DXGI_FORMAT_R32G32B32_FLOAT: return 12;
        case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
        case DXGI_FORMAT_B5G5R5A1_UNORM: return 2;
        case DXGI_FORMAT_B5G6R5_UNORM: return 2;
        case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return 4;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return 3;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return 3;
        case DXGI_FORMAT_R16_UNORM: return 2;
        case DXGI_FORMAT_R16_FLOAT: return 2;
        case DXGI_FORMAT_R16G16_FLOAT: return 4;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;
        // TODO : add if needed
        default: assert(false); return 1;
    }
}

std::string D3D11ReturnCodeToStr(HRESULT rs)
{
    // See also : windows-win32-direct3d11.pdf - Direct3D 11 Return Codes
    switch (rs)
    {
        case D3D11_ERROR_FILE_NOT_FOUND: return "D3D11_ERROR_FILE_NOT_FOUND";
        case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
        case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
        case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: return "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
        case D3DERR_INVALIDCALL: return "D3DERR_INVALIDCALL";
        case E_FAIL: return "E_FAIL";
        case E_INVALIDARG: return "E_INVALIDARG";
        case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
        case E_NOTIMPL: return "E_NOTIMPL";
        case S_FALSE: return "S_FALSE";
        case S_OK: return "S_OK";
        default:
            return "UNKOWN";
    }
}

} // namespace Mmp