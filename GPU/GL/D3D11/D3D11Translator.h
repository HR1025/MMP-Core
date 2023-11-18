//
// D3D11Translator.h
//
// Library: GPU
// Package: GL
// Module:  D3D11
// 

#pragma once

#include "D3D11Common.h"

namespace Mmp
{

D3D11_COMPARISON_FUNC GLComparisonTypeToD3D11Type(Comparison comparison);

D3D11_STENCIL_OP GLStencilOpToD3D11Type(StencilOp op);

D3D11_BLEND_OP GLBlendOpToOpenD3D11Type(BlendOp op);

D3D11_TEXTURE_ADDRESS_MODE GLTextureAddressModeToD3D11Type(TextureAddressMode mode);

D3D11_BLEND GLBlendFactorToOpenD3D11Type(BlendFactor bf);

D3D11_CULL_MODE GLCullModeToOpenD3D11Type(CullMode mode);

D3D11_PRIMITIVE_TOPOLOGY GLPrimitiveToOpenD3D11Type(Primitive primitive);

} // namespace Mmp