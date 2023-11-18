//
// DXGICommon.h
//
// Library: GPU
// Package: Windows
// Module:  DXGI
// 
#pragma once

#include "WindowsCommon.h"

#include "dxgi.h"
#include "d3d11.h"
#include "d3dcompiler.h"

typedef HRESULT (WINAPI *LPCREATEDXGIFACTORY)(REFIID, void **);
typedef HRESULT (WINAPI *LPD3D11CREATEDEVICE)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT32, D3D_FEATURE_LEVEL *, UINT, UINT32, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);

namespace Mmp
{

} // namespace Mmp 