#include "OpenGLTranslator.h"

#include <cassert>
#include <unordered_map>

namespace Mmp
{

GLenum GLShaderStageToOpenGLType(SL::ShaderStage stage)
{
    static std::unordered_map<SL::ShaderStage, GLenum> kLookup = 
    {
        {SL::ShaderStage::VERTEX, GL_VERTEX_SHADER},
        {SL::ShaderStage::FRAGMENT, GL_FRAGMENT_SHADER},
        {SL::ShaderStage::COMPUTE, GL_COMPUTE_SHADER},
        {SL::ShaderStage::GEOMETRY, GL_GEOMETRY_SHADER},
    };
    if (kLookup.count(stage) == 1)
    {
        return kLookup[stage];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLenum GLTextureTypeToOpenGLType(TextureType type)
{
    static std::unordered_map<TextureType, GLenum> kLookup = 
    {
        {TextureType::LINEAR1D, GL_TEXTURE_1D},
        {TextureType::LINEAR2D, GL_TEXTURE_2D},
        {TextureType::LINEAR3D, GL_TEXTURE_3D},
        {TextureType::CUBE, GL_TEXTURE_CUBE_MAP},
        {TextureType::ARRAY1D, GL_TEXTURE_1D_ARRAY},
        {TextureType::ARRAY2D, GL_TEXTURE_2D_ARRAY}
    };
    if (kLookup.count(type) == 1)
    {
        return kLookup[type];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLenum GLComparisonTypeToOpenGLType(Comparison comparison)
{

    static std::unordered_map<Comparison, GLenum> kLookup = 
    {
        {Comparison::NEVER, GL_NEVER},
        {Comparison::LESS, GL_LESS},
        {Comparison::EQUAL, GL_EQUAL},
        {Comparison::LESS_EQUAL, GL_LEQUAL},
        {Comparison::GREATER, GL_GREATER},
        {Comparison::NOT_EQUAL, GL_NOTEQUAL},
        {Comparison::GREATER_EQUAL, GL_GEQUAL},
        {Comparison::ALWAYS, GL_ALWAYS},        
    };
    if (kLookup.count(comparison) == 1)
    {
        return kLookup[comparison];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLenum GLLogicOpToOpenGLType(LogicOp op)
{
    static std::unordered_map<LogicOp, GLenum> kLookup = 
    {
        {LogicOp::CLEAR, GL_CLEAR},
        {LogicOp::SET, GL_SET},
        {LogicOp::COPY, GL_COPY},
        {LogicOp::COPY_INVERTED, GL_COPY_INVERTED},
        {LogicOp::NOOP, GL_NOOP},
        {LogicOp::INVERT, GL_INVERT},
        {LogicOp::AND, GL_AND},
        {LogicOp::NAND, GL_NAND},
        {LogicOp::OR, GL_OR},
        {LogicOp::NOR, GL_NOR},
        {LogicOp::XOR, GL_XOR},
        {LogicOp::EQUIV, GL_EQUIV},
        {LogicOp::AND_REVERSE, GL_AND_REVERSE},
        {LogicOp::AND_INVERTED, GL_AND_INVERTED},        
        {LogicOp::OR_REVERSE, GL_OR_REVERSE},     
        {LogicOp::OR_INVERTED, GL_OR_INVERTED},     
    };
    if (kLookup.count(op) == 1)
    {
        return kLookup[op];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLuint GLStencilOpToOpenGLType(StencilOp op)
{
    static std::unordered_map<StencilOp, GLuint> kLookup = 
    {
        {StencilOp::KEEP, GL_KEEP},
        {StencilOp::ZERO, GL_ZERO},
        {StencilOp::REPLACE, GL_REPLACE},
        {StencilOp::INCREMENT_AND_CLAMP, GL_INCR},
        {StencilOp::DECREMENT_AND_CLAMP, GL_DECR},
        {StencilOp::INVERT, GL_INVERT},
        {StencilOp::INCREMENT_AND_WRAP, GL_INCR_WRAP},
        {StencilOp::DECREMENT_AND_WRAP, GL_DECR_WRAP}, 
    };
    if (kLookup.count(op) == 1)
    {
        return kLookup[op];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLuint GLBlendFactorToOpenGLType(BlendFactor bf)
{
    static std::unordered_map<BlendFactor, GLuint> kLookup = 
    {
        {BlendFactor::ZERO, GL_ZERO},
        {BlendFactor::ONE, GL_ONE},
        {BlendFactor::SRC_COLOR, GL_SRC_COLOR},
        {BlendFactor::ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR},
        {BlendFactor::DST_COLOR, GL_DST_COLOR},
        {BlendFactor::ONE_MINUS_DST_COLOR, GL_ONE_MINUS_DST_COLOR},
        {BlendFactor::SRC_ALPHA, GL_SRC_ALPHA},
        {BlendFactor::ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA},
        {BlendFactor::DST_ALPHA, GL_DST_ALPHA},
        {BlendFactor::ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA},
        {BlendFactor::CONSTANT_COLOR, GL_CONSTANT_COLOR},
        {BlendFactor::ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR},
        {BlendFactor::CONSTANT_ALPHA, GL_CONSTANT_ALPHA},
        {BlendFactor::ONE_MINUS_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA},
        {BlendFactor::SRC1_COLOR, GL_SRC1_COLOR},
        {BlendFactor::ONE_MINUS_SRC1_COLOR, GL_ONE_MINUS_SRC1_COLOR},
        {BlendFactor::SRC1_ALPHA, GL_SRC1_ALPHA},
        {BlendFactor::ONE_MINUS_SRC1_ALPHA, GL_ONE_MINUS_SRC1_ALPHA},
    };
    if (kLookup.count(bf) == 1)
    {
        return kLookup[bf];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLuint GLBlendOpToOpenGLType(BlendOp op)
{
    static std::unordered_map<BlendOp, GLuint> kLookup = 
    {
        {BlendOp::ADD, GL_FUNC_ADD},
        {BlendOp::SUBTRACT, GL_FUNC_SUBTRACT},
        {BlendOp::REV_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT},
        {BlendOp::MIN, GL_MIN},
        {BlendOp::MAX, GL_MAX}
    };
    if (kLookup.count(op) == 1)
    {
        return kLookup[op];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }
}

GLuint GLTextureAddressModeToOpenGLType(TextureAddressMode mode)
{
    static std::unordered_map<TextureAddressMode, GLuint> kLookup = 
    {
        {TextureAddressMode::REPEAT, GL_REPEAT},
        {TextureAddressMode::REPEAT_MIRROR, GL_MIRRORED_REPEAT},
        {TextureAddressMode::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
        {TextureAddressMode::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
    };
    if (kLookup.count(mode) == 1)
    {
        return kLookup[mode];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }   
}

GLuint GLTextureFilterToOpenGLType(TextureFilter tf)
{
    static std::unordered_map<TextureFilter, GLuint> kLookup = 
    {
        {TextureFilter::NEAREST, GL_NEAREST},
        {TextureFilter::LINEAR, GL_LINEAR},
    };
    if (kLookup.count(tf) == 1)
    {
        return kLookup[tf];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }  
}

GLuint GLFacingToOpenGLType(Facing face)
{
    static std::unordered_map<Facing, GLuint> kLookup = 
    {
        {Facing::CW, GL_CW},
        {Facing::CCW, GL_CCW},
    };
    if (kLookup.count(face) == 1)
    {
        return kLookup[face];
    }
    else
    {
        assert(false);
        return GL_NONE;
    }  
}

GLuint GLCullModeToOpenGLType(CullMode mode)
{
    static std::unordered_map<CullMode, GLuint> kLookup = 
    {
        {CullMode::FRONT, GL_FRONT},
        {CullMode::BACK, GL_BACK},
        {CullMode::FRONT_AND_BACK, GL_FRONT_AND_BACK},
    };
    if (kLookup.count(mode) == 1)
    {
        return kLookup[mode];
    }
    else
    {
        // Hint : CullMode::NONE not support
        assert(false);
        return GL_NONE;
    }  
}

GLuint GLPrimitiveToOpenGLType(Primitive primitive)
{
    static std::unordered_map<Primitive, GLuint> kLookup = 
    {
        {Primitive::POINT_LIST, GL_POINTS},
        {Primitive::LINE_LIST, GL_LINES},
        {Primitive::LINE_STRIP, GL_LINE_STRIP},
        {Primitive::TRIANGLE_LIST, GL_TRIANGLES},
        {Primitive::TRIANGLE_STRIP, GL_TRIANGLE_STRIP},
        {Primitive::TRIANGLE_FAN, GL_TRIANGLE_FAN},
    };
    if (kLookup.count(primitive) == 1)
    {
        return kLookup[primitive];
    }
    else
    {
        // Hint : CullMode::NONE not support
        assert(false);
        return GL_NONE;
    }  
}

GLuint GenerateMipFilterOpenGLType(TextureFilter minTf, TextureFilter mipTf)
{
    switch (minTf)
    {
        case TextureFilter::NEAREST:
        {
            if (mipTf == TextureFilter::NEAREST)
            {
                return GL_NEAREST_MIPMAP_NEAREST;
            }
            else if (mipTf == TextureFilter::LINEAR)
            {
                return GL_NEAREST_MIPMAP_LINEAR;
            }
            else
            {
                assert(false);
                return GL_NONE;
            }
            break;
        }
        case TextureFilter::LINEAR:
        {
            if (mipTf == TextureFilter::NEAREST)
            {
                return GL_LINEAR_MIPMAP_NEAREST;
            }
            else if (mipTf == TextureFilter::LINEAR)
            {
                return GL_LINEAR_MIPMAP_LINEAR;
            }
            else
            {
                assert(false);
                return GL_NONE;
            }
            break;
        }
        default: assert(false); return GL_NONE;
    }
    assert(false);
}

} // namespace Mmp