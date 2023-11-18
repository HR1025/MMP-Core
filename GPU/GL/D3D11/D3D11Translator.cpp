#include "D3D11Translator.h"

#include <cassert>

namespace Mmp
{

D3D11_COMPARISON_FUNC GLComparisonTypeToD3D11Type(Comparison comparison)
{
    switch (comparison)
    {
        case Comparison::NEVER: return D3D11_COMPARISON_NEVER;
        case Comparison::LESS: return D3D11_COMPARISON_LESS;
        case Comparison::EQUAL: return D3D11_COMPARISON_EQUAL;
        case Comparison::LESS_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
        case Comparison::GREATER: return D3D11_COMPARISON_GREATER;
        case Comparison::NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
        case Comparison::GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
        case Comparison::ALWAYS: return D3D11_COMPARISON_ALWAYS;
        default:
            assert(false);
            return D3D11_COMPARISON_NEVER;
    }
}

D3D11_STENCIL_OP GLStencilOpToD3D11Type(StencilOp op)
{
    switch (op)
    {
        case StencilOp::KEEP: return D3D11_STENCIL_OP_KEEP;
        case StencilOp::ZERO: return D3D11_STENCIL_OP_ZERO;
        case StencilOp::REPLACE: return D3D11_STENCIL_OP_REPLACE;
        case StencilOp::INCREMENT_AND_CLAMP: return D3D11_STENCIL_OP_INCR_SAT;
        case StencilOp::DECREMENT_AND_CLAMP: return D3D11_STENCIL_OP_DECR_SAT;
        case StencilOp::INVERT: return D3D11_STENCIL_OP_INVERT;
        case StencilOp::INCREMENT_AND_WRAP: return D3D11_STENCIL_OP_INCR;
        case StencilOp::DECREMENT_AND_WRAP: return D3D11_STENCIL_OP_DECR;
        default:
            assert(false);
            return D3D11_STENCIL_OP_KEEP;
    }
}

D3D11_BLEND_OP GLBlendOpToOpenD3D11Type(BlendOp op)
{
    switch (op)
    {
        case BlendOp::ADD: return D3D11_BLEND_OP_ADD;
        case BlendOp::SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
        case BlendOp::REV_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
        case BlendOp::MIN: return D3D11_BLEND_OP_MIN;
        case BlendOp::MAX: return D3D11_BLEND_OP_MAX;
        default:
            assert(false);
            return D3D11_BLEND_OP_ADD;
    }
}

D3D11_TEXTURE_ADDRESS_MODE GLTextureAddressModeToD3D11Type(TextureAddressMode mode)
{
    switch (mode)
    {
        case TextureAddressMode::REPEAT: return D3D11_TEXTURE_ADDRESS_WRAP;
        case TextureAddressMode::REPEAT_MIRROR: return D3D11_TEXTURE_ADDRESS_MIRROR;
        case TextureAddressMode::CLAMP_TO_EDGE: return D3D11_TEXTURE_ADDRESS_CLAMP;
        case TextureAddressMode::CLAMP_TO_BORDER: return D3D11_TEXTURE_ADDRESS_BORDER;
        default:
            assert(false);
            return D3D11_TEXTURE_ADDRESS_WRAP;
    }
}

D3D11_BLEND GLBlendFactorToOpenD3D11Type(BlendFactor bf)
{
    switch (bf)
    {
        case BlendFactor::ZERO: return D3D11_BLEND_ZERO;
        case BlendFactor::ONE: return D3D11_BLEND_ONE;
        case BlendFactor::SRC_COLOR: return D3D11_BLEND_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
        case BlendFactor::DST_COLOR: return D3D11_BLEND_DEST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR: return D3D11_BLEND_DEST_COLOR;
        case BlendFactor::SRC_ALPHA: return D3D11_BLEND_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DST_ALPHA: return D3D11_BLEND_DEST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
        case BlendFactor::CONSTANT_COLOR: return D3D11_BLEND_BLEND_FACTOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return D3D11_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::CONSTANT_ALPHA: return D3D11_BLEND_BLEND_FACTOR;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return D3D11_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::SRC1_COLOR: return D3D11_BLEND_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR: return D3D11_BLEND_INV_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA: return D3D11_BLEND_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA: return D3D11_BLEND_INV_SRC1_ALPHA;
        default:
            assert(false);
            return D3D11_BLEND_ZERO;
    }
}

D3D11_CULL_MODE GLCullModeToOpenD3D11Type(CullMode mode)
{
    switch (mode)
    {
        case CullMode::BACK: return D3D11_CULL_BACK;
        case CullMode::FRONT: return D3D11_CULL_FRONT;
        case CullMode::FRONT_AND_BACK: return D3D11_CULL_NONE;
        case CullMode::NONE: return D3D11_CULL_NONE;
        default:
            assert(false);
            return D3D11_CULL_NONE;
    }
}

D3D11_PRIMITIVE_TOPOLOGY GLPrimitiveToOpenD3D11Type(Primitive primitive)
{
    switch (primitive)
    {
        case Primitive::POINT_LIST: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case Primitive::LINE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case Primitive::LINE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case Primitive::TRIANGLE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case Primitive::TRIANGLE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        case Primitive::TRIANGLE_FAN: return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        case Primitive::PATCH_LIST: return D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
        case Primitive::LINE_LIST_ADJ: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
        case Primitive::LINE_STRIP_ADJ: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
        default:
            assert(false);
            return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

} // namespace Mmp