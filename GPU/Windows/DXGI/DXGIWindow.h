//
// DXGIWindow.h
//
// Library: GPU
// Package: Windows
// Module:  DXGI
// 

#pragma once

#include <vector>

#include "DXGICommon.h"
#include "AbstractWindows.h"

namespace Mmp
{

/**
 * @note 基于 DXGI 实现 (runtime loading)
 */
class DXGIWindow : public AbstractWindows
{
public:
    DXGIWindow();
    ~DXGIWindow();
public:
    bool Open() override;
    bool Close() override;
    bool BindRenderThread(bool bind) override;
    bool SetAPIType(APIType type) override;
    bool SetRenderMode(bool onScreen) override;
    WindowInfo GetInfo() override;
    void  Swap() override;
public:
    /**
     * @note
     *       key                    :    value
     *       ID3D11Device           :    ID3D11Device*
     *       ID3D11DeviceContext    :    ID3D11DeviceContext*
     *       D3DCompile             :    _D3DCompile
     *       Width                  :    _width
     *       Height                 :    _height
     *       BackBufferTexture      :    _backBufferTexture
     *       BackRenderTargetView   :    _backRenderTargetView 
     *       BackDepthTexture       :    _backDepthTexture
     *       BackDepthStencilView   :    _backDepthStencilView
     */
    Any Get(const std::string& key) override;
private:
    bool LoadD3D11();
    void UnLoadD3D11();
    bool CreateDevice();
    void DestroyDevice();
    bool CreateSwapChain();
    void DestroySwapChain();
    void UpdateScreenInfo();
    void CreateWinWindow();
    void GetBackBuffer();
private:
    bool _isOpened;        // HINT : 目前 DXGIWindow 开启后就不再关闭,简化处理流程
                           // TODO : 是否考虑支持 close 和 reopen ???
    bool _isOnScreen;
private:
    bool                                _isLoaded;
    LPCREATEDXGIFACTORY                 _CreateDXGIFactory1;
    LPD3D11CREATEDEVICE                 _D3D11CreateDevice;
    pD3DCompile                         _D3DCompile;
private:
    HINSTANCE                           _hInstance;
    HWND                                _hWnd;       // TODO : 支持外部导入
private:
    int                                 _displayScreenWidth;
    int                                 _displayScreenHeight;
    int                                 _virtualScreenWidth;
    int                                 _virtualScreenHeight;
private:
    int                                 _width;
    int                                 _height;
private:
    ID3D11Device*                       _device;
    ID3D11DeviceContext*                _context;
    D3D_FEATURE_LEVEL                   _featureLevel;
private:
    IDXGISwapChain*                     _swapChain;
    ID3D11Texture2D*                    _backBufferTexture;
    ID3D11RenderTargetView*             _backRenderTargetView;
    ID3D11Texture2D*                    _backDepthTexture;
    ID3D11DepthStencilView*             _backDepthStencilView;
};

} // namespace Mmp