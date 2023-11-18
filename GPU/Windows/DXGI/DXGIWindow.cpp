#include "DXGIWindow.h"

#include <cassert>

#include "WindowUtils.h"

#include <Poco/SharedLibrary.h>
#include <Poco/UnicodeConverter.h>

static Poco::SharedLibrary gD3D11Loader;
static Poco::SharedLibrary gDXGILoader;
static Poco::SharedLibrary gD3DComplieLoader;

namespace Mmp
{

DXGIWindow::DXGIWindow()
{
    _isOpened = false;
    _isOnScreen = true;
    _width = -1;
    _height = -1;
    _isLoaded = false;
    _CreateDXGIFactory1 = nullptr;
    _D3D11CreateDevice = nullptr;
    _D3DCompile = nullptr;
    _device = nullptr;
    _swapChain = nullptr;
    _context = nullptr;
    _featureLevel = D3D_FEATURE_LEVEL_11_0;
    _displayScreenWidth = -1;
    _displayScreenHeight = -1;
    _virtualScreenWidth = -1;
    _virtualScreenHeight = -1;

    _backBufferTexture = nullptr;
    _backRenderTargetView = nullptr;
    _backDepthTexture = nullptr;
    _backDepthStencilView = nullptr;

    _hInstance = Win32GetInstance();
}

DXGIWindow::~DXGIWindow()
{
    // TODO : 
}

bool DXGIWindow::Open()
{
    if (_isOpened)
    {
        return true;
    }
    UpdateScreenInfo();
    if (!LoadD3D11())
    {
        goto END;
    }
    if (!CreateDevice())
    {
        goto END1;
    }
    if (!CreateSwapChain())
    {
        goto END2;
    }
    GetBackBuffer();
    _isOpened = true;
    return true;
/* END3: */
    DestroySwapChain();
END2:
    DestroyDevice();
END1:
    UnLoadD3D11();
END:
    return false;
}

bool DXGIWindow::Close()
{
    // Hint : not support close, only opened once
    // See also : _isOpened
    return true;
}

bool DXGIWindow::BindRenderThread(bool bind)
{
    return true;
}

bool DXGIWindow::SetAPIType(APIType type)
{
    assert(type == APIType::D3D11);
    return true;
}

bool DXGIWindow::SetRenderMode(bool onScreen)
{
    _isOnScreen = onScreen;
    return true;
}

WindowInfo DXGIWindow::GetInfo()
{
    WindowInfo info;
    info.apiType = APIType::D3D11;
    info.vMajor = 11;
    info.vMinor = 0;
    return info;
}

void DXGIWindow::Swap()
{
    if (_isOnScreen)
    {
        _swapChain->Present(0, 0);
    }
}

Any DXGIWindow::Get(const std::string& key)
{
    if (key == "ID3D11Device")
    {
        return _device;
    }
    else if (key == "ID3D11DeviceContext")
    {
        return _context;
    }
    else if (key == "D3DCompile")
    {
        return _D3DCompile;
    }
    else if (key == "Width")
    {
        return _width;
    }
    else if (key == "Height")
    {
        return _height;
    }
    else if (key == "BackBufferTexture")
    {
        return _backBufferTexture;
    }
    else if (key == "BackRenderTargetView")
    {
        return _backRenderTargetView;
    }
    else if (key == "BackDepthTexture")
    {
        return _backDepthTexture;
    }
    else if (key == "BackDepthStencilView")
    {
        return _backDepthStencilView;
    }
    else
    {
        return AbstractWindows::Get(key);
    }
}

bool DXGIWindow::LoadD3D11()
{
    WIN_LOG_INFO << "LoadD3D11";
    WIN_LOG_INFO << "Load D3D11 related runtime depend";
    try 
    {
        if (!gD3D11Loader.isLoaded())
        {
            gD3D11Loader.load("d3d11.dll");
        }
        WIN_LOG_INFO << "-- Load d3d11.dll successfully";
    }
    catch (const Poco::LibraryLoadException& ex)
    {
        WIN_LOG_ERROR << "-- Load d3d11.dll fail, error msg is: " << ex.message();
        assert(false);
        return false;
    }
    try 
    {
        if (!gDXGILoader.isLoaded())
        {
            gDXGILoader.load("dxgi.dll");
        }
        WIN_LOG_INFO << "-- Load dxgi.dll successfully";
    }
    catch (const Poco::LibraryLoadException& ex)
    {
        WIN_LOG_ERROR << "-- Load dxgi.dll fail, error msg is: " << ex.message();
        assert(false);
        return false;
    }
    try 
    {
        if (!gD3DComplieLoader.isLoaded())
        {
            gD3DComplieLoader.load("D3dcompiler_42.dll");
        }
        WIN_LOG_INFO << "-- Load D3dcompiler_42.dll successfully";
    }
    catch (const Poco::LibraryLoadException& ex)
    {
        WIN_LOG_ERROR << "-- Load D3dcompiler_42.dll fail, error msg is: " << ex.message();
        assert(false);
        return false;
    }
    WIN_LOG_INFO << "Load D3D11 related runtime symbol";
    if (gD3D11Loader.hasSymbol("D3D11CreateDevice"))
    {
        _D3D11CreateDevice = (LPD3D11CREATEDEVICE)gD3D11Loader.getSymbol("D3D11CreateDevice");
        WIN_LOG_INFO << "-- Load symbol D3D11CreateDevice successfully";
    }
    else
    {
        WIN_LOG_ERROR << "-- Load symbol D3D11CreateDevice successfully";
        assert(false);
        return false;
    }
    if (gDXGILoader.hasSymbol("CreateDXGIFactory1"))
    {
        _CreateDXGIFactory1 = (LPCREATEDXGIFACTORY)gDXGILoader.getSymbol("CreateDXGIFactory1");
        WIN_LOG_INFO << "-- Load symbol CreateDXGIFactory1 successfully";
    }
    else
    {
        WIN_LOG_ERROR << "-- Load symbol CreateDXGIFactory1 successfully";
        assert(false);
        return false;
    }
    if (gD3DComplieLoader.hasSymbol("D3DCompile"))
    {
        _D3DCompile = (pD3DCompile)gD3DComplieLoader.getSymbol("D3DCompile");
        WIN_LOG_INFO << "-- Load symbol D3DCompile successfully";
    }
    else
    {
        WIN_LOG_ERROR << "-- Load symbol D3DCompile successfully";
        assert(false);
        return false;
    }
    _isLoaded = true;
    return true;
}

void DXGIWindow::UnLoadD3D11()
{
    WIN_LOG_INFO << "UnLoadD3D11";
    _isLoaded = false;
    _CreateDXGIFactory1 = nullptr;
    _D3D11CreateDevice = nullptr;
    _D3DCompile = nullptr;
    if (gD3D11Loader.isLoaded())
    {
        gD3D11Loader.unload();
    }
    if (gDXGILoader.isLoaded())
    {
        gDXGILoader.unload();
    }
    if (gD3DComplieLoader.isLoaded())
    {
        gD3DComplieLoader.unload();
    }
}

bool DXGIWindow::CreateDevice()
{
    std::vector<IDXGIAdapter*> adapters;
    IDXGIFactory* DXGIFactory = nullptr;
    WIN_LOG_INFO << "CreateDevice";
    if (SUCCEEDED(_CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&DXGIFactory)))
    {
        IDXGIAdapter* adapter = nullptr;
        // See also : windows-win32-direct3d11.pdf - How To: Enumerate Adapters
        for (UINT i = 0; DXGIFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
        {
            DXGI_ADAPTER_DESC desc = {};
            adapter->GetDesc(&desc);
            WIN_LOG_INFO << "DXGIAdapter (" << i << ")";
            {
                std::string description;
                Poco::UnicodeConverter::convert(desc.Description, description);
                WIN_LOG_INFO << "-- Description is: " << description;
            }
            WIN_LOG_INFO << "-- Vendor id is: " << desc.VendorId;
            WIN_LOG_INFO << "-- Device id is: " << desc.DeviceId;
            WIN_LOG_INFO << "-- Sub System id is: " << desc.SubSysId;
            adapters.push_back(adapter);
        }
        if (adapters.empty())
        {
            DXGIFactory->Release();
            assert(false);
            return false;
        }
    }
    else
    {
        WIN_LOG_ERROR << "CreateDXGIFactory1 fail";
        assert(false);
        return false;
    }
    // TODO : should choosen by user or simply choose the first ???
    IDXGIAdapter* chooseAdapter = adapters.front();
    {
        UINT createDeviceFlags = 0;
#ifdef ENABLE_D3D11_DEBUG
        createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif /* ENABLE_D3D11_DEBUG */
        static const D3D_DRIVER_TYPE driverTypes[] = {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        static const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
        };
        if (FAILED(_D3D11CreateDevice(chooseAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags,
            (D3D_FEATURE_LEVEL *)featureLevels, UINT(1), D3D11_SDK_VERSION, &_device, &_featureLevel, &_context
        )))
        {
            WIN_LOG_ERROR << "3D11CreateDevice fail";
            assert(false);
            return false;
        }
    }
    for (const auto& adapter : adapters)
    {
        adapter->Release();
    }
    DXGIFactory->Release();
    return true;
}

void DXGIWindow::DestroyDevice()
{
    WIN_LOG_INFO << "CloseDevice";
    // HINT : not support for now
    assert(false);
}

bool DXGIWindow::CreateSwapChain()
{
    IDXGIFactory1* DXGIFactory = nullptr;
    IDXGIDevice* device = nullptr;
    IDXGIAdapter* adapter = nullptr;
    if (_isOnScreen)
    {
        WIN_LOG_INFO << "CreateSwapChain";
        if (FAILED(_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&device)))
        {
            WIN_LOG_ERROR << "IDXGIDevice::QueryInterface fail";
            assert(false);
            return false;
        }
        if (FAILED(device->GetAdapter(&adapter)))
        {
            WIN_LOG_ERROR << "IDXGIDevice::GetAdapter fail";
            assert(false);
            return false;
        }
        if (FAILED(adapter->GetParent(__uuidof(IDXGIFactory1), (void**)&DXGIFactory)))
        {
            WIN_LOG_ERROR << "IDXGIAdapter::GetParent fail";
            assert(false);
            return true;
        }
        CreateWinWindow();
        DXGI_SWAP_CHAIN_DESC desc = {};
        desc.BufferCount = 1;
        desc.BufferDesc.Width = DXGI_SCREEN_WIDTH;
        desc.BufferDesc.Height = DXGI_SCREEN_HEIGHT;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.RefreshRate.Numerator = 60;
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	    desc.OutputWindow = _hWnd;
	    desc.SampleDesc.Count = 1;
	    desc.SampleDesc.Quality = 0;
	    desc.Windowed = TRUE;
        if (FAILED(DXGIFactory->CreateSwapChain(_device, &desc, &_swapChain)))
        {
            WIN_LOG_ERROR << "IDXGIFactory1::CreateSwapChain fail";
            assert(false);
        }
        DXGIFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER);
        DXGIFactory->Release();
    }

    return true;
}

void DXGIWindow::DestroySwapChain()
{
    WIN_LOG_INFO << "DestroySwapChain";
}

void DXGIWindow::UpdateScreenInfo()
{
    _virtualScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    _virtualScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    _displayScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    _displayScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    WIN_LOG_INFO << "Virtual Screen " << _virtualScreenWidth << "x" << _virtualScreenHeight;
    WIN_LOG_INFO << "Display Screen " << _displayScreenWidth << "x" << _displayScreenHeight;
}

void DXGIWindow::CreateWinWindow()
{
    WIN_LOG_INFO << "CreateWinWindow";
    _hWnd = Win32CreateWindow(MMP_NAME, WS_OVERLAPPEDWINDOW, DXGI_SCREEN_WIDTH, DXGI_SCREEN_HEIGHT, _hInstance);;
    assert(_hWnd);
    Win32ShowWindow(_hWnd, SW_SHOW); // TODO : a better way
}

void DXGIWindow::GetBackBuffer()
{
    if (_backBufferTexture)
    {
        _backBufferTexture->Release();
        _backBufferTexture = nullptr;
    }
    if (_backRenderTargetView)
    {
        _backRenderTargetView->Release();
        _backRenderTargetView = nullptr;
    }
    if (_backDepthTexture)
    {
        _backDepthTexture->Release();
        _backDepthTexture = nullptr;
    }
    if (_backDepthStencilView)
    {
        _backDepthStencilView->Release();
        _backDepthStencilView = nullptr;
    }
    if (_isOnScreen)
    {
        if (FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_backBufferTexture))))
        {
            WIN_LOG_ERROR << "IDXGISwapChain::GetBuffer Fail";
            assert(false);
            return;
        }
    }
    else
    {
        _width = DXGI_SCREEN_WIDTH;
        _height = DXGI_SCREEN_HEIGHT;
        D3D11_TEXTURE2D_DESC colorDesc;
        colorDesc.Width = (UINT)_width;
        colorDesc.Height = (UINT)_height;
        colorDesc.MipLevels = 0;
        colorDesc.ArraySize = 1;
        colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorDesc.SampleDesc.Count = 1;
        colorDesc.SampleDesc.Quality = 0;
        colorDesc.Usage = D3D11_USAGE_DEFAULT;
        colorDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        colorDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        colorDesc.CPUAccessFlags = 0;
        if (FAILED(_device->CreateTexture2D(&colorDesc, nullptr, &_backBufferTexture)))
        {
            WIN_LOG_ERROR << "ID3D11Device::CreateTexture2D Fail";
        }
    }
    {
        D3D11_TEXTURE2D_DESC bbDesc = {};
	    _backBufferTexture->GetDesc(&bbDesc);
        _width = bbDesc.Width;
        _height = bbDesc.Height;
        if (FAILED(_device->CreateRenderTargetView(_backBufferTexture, nullptr, &_backRenderTargetView)))
        {
            WIN_LOG_ERROR << "ID3D11Device::CreateRenderTargetView Fail";
            assert(false);
            return;
        }
    }
    {
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = (UINT)_width;
        depthDesc.Height = (UINT)_height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.CPUAccessFlags = 0;
        depthDesc.MiscFlags = 0;
        if (FAILED(_device->CreateTexture2D(&depthDesc, nullptr, &_backDepthTexture)))
        {
            WIN_LOG_ERROR << "ID3D11Device::CreateTexture2D Fail";
            assert(false);
            return;
        }
    }
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
        if (FAILED(_device->CreateDepthStencilView(_backDepthTexture, &dsvDesc, &_backDepthStencilView)))
        {
            WIN_LOG_ERROR << "ID3D11Device::CreateDepthStencilView Fail";
            assert(false);
            return;
        }
    }
}

} // namespace Mmp