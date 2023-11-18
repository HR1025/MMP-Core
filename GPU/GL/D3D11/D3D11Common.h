//
// D3D11Common.h
//
// Library: GPU
// Package: GL
// Module:  D3D11
//

#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

#include <windows.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX11core.h>
#include <d3dcompiler.h>

#include "Common/Any.h"
#include "GLCommon.h"
#include "GLContexDesc.h"
#include "GLContex.h"

namespace Mmp
{

#define D3D11_MAX_BOUND_TEXTURES 8
#define D3D11_MAX_VERTEX_BUFFER_SIZE 4

DXGI_FORMAT DataFormatToD3D11(DataFormat format);
size_t DxgiFormatSize(DXGI_FORMAT format);

std::string D3D11ReturnCodeToStr(HRESULT rs);

} // namespace Mmp