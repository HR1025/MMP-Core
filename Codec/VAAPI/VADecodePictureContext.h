//
// VADecodePictureContext.h
//
// Library: Codec
// Package: VAAPI
// Module:  H264
// 

#pragma once

#include "VACommon.h"

namespace Mmp
{
namespace Codec
{

class VADecoderContext;

class VADecodePictureContext
{
public:
    using ptr = std::shared_ptr<VADecodePictureContext>;
public:
    VADecodePictureContext();
    ~VADecodePictureContext();
public:
    void SetDecoderContext(std::shared_ptr<VADecoderContext> context);
    std::shared_ptr<VADecoderContext> GetDecoderContext();
public:
    VASurfaceID                 surface;
    std::vector<VABufferID>     paramBuffers;
    std::vector<VABufferID>     sliceBuffers;
private:
    std::shared_ptr<VADecoderContext>  _context;
};

} // namespace Codec
} // namespace Mmp