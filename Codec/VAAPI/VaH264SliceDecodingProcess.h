//
// VaH264SliceDecodingProcess.h
//
// Library: Codec
// Package: VAAPI
// Module:  H264
// 

#pragma once

#include "VADecoder.h"
#include "VADecodePictureContext.h"
#include "H264/H264Deserialize.h"
#include "H264/H264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

class VAH264DecodePictureContext : public H264PictureContext, public VADecodePictureContext
{
public:
    using ptr = std::shared_ptr<VAH264DecodePictureContext>;
public:
    VAH264DecodePictureContext();
    ~VAH264DecodePictureContext();
};

class VaH264SliceDecodingProcess : public H264SliceDecodingProcess
{
protected:
    H264PictureContext::ptr CreatePictureContext() override;
};

} // namespace Codec
} // namespace Mmp