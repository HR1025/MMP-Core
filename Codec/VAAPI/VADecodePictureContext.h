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

class VADecoder;

class VADecodePictureContext
{
public:
    using ptr = std::shared_ptr<VADecodePictureContext>;
public:
    VADecodePictureContext();
    ~VADecodePictureContext();
public:
    void SetVADecoder(std::shared_ptr<VADecoder> decoder);
public:
    VASurfaceID                 surface;
    std::vector<VABufferID>     paramBuffers;
    std::vector<VABufferID>     sliceBuffers;
private:
    std::shared_ptr<VADecoder>  _decoder;
};

} // namespace Codec
} // namespace Mmp