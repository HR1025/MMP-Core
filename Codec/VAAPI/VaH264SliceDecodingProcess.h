//
// VaH264SliceDecodingProcess.h
//
// Library: Codec
// Package: VAAPI
// Module:  H264
// 

#pragma once

#include "VADecoder.h"
#include "H264/H264Deserialize.h"
#include "H264/H264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

class VaH264DecodePictureContext : public H264PictureContext
{
public:
    using ptr = std::shared_ptr<VaH264DecodePictureContext>;
public:
    VaH264DecodePictureContext();
    ~VaH264DecodePictureContext();
public:
    VASurfaceID              surface;
    std::vector<VABufferID>  paramBuffers;
    std::vector<VABufferID>  sliceBuffers;
};

class VaH264SliceDecodingProcess : public H264SliceDecodingProcess
{
protected:
    H264PictureContext::ptr CreatePictureContext() override;
};

} // namespace Codec
} // namespace Mmp