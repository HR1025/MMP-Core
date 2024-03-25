#include "GLContex.h"

#include <cassert>

namespace Mmp
{

int FrameBuffer::Width()
{
    return _width;
}

int FrameBuffer::Height()
{
    return _height;
}

int FrameBuffer::Layers()
{
    return _layers;
}

int FrameBuffer::MultiSampleLevel()
{
    return _layers;
}

int Texture::Width()
{
    return _width;
}

int Texture::Height()
{
    return _height;
}

int Texture::Depth()
{
    return _depth;
}

GlTextureFlags Texture::Flags()
{
    return _flags;
}

} // namespace Mmp