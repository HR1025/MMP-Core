//
// VAH264Decoder.h
//
// Library: Codec
// Package: VAAPI
// Module:  H264
// 

#pragma once

#include "VADecoder.h"
#include "H264/H264Deserialize.h"
#include "VaH264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

class VAH264Decoder : public VADecoder
{
public:
    using ptr = std::shared_ptr<VAH264Decoder>();
public:
    VAH264Decoder();
    ~VAH264Decoder() = default;
public: /* override AbstractDecoder function */
    bool Push(AbstractPack::ptr pack) override;
private:
    void StartFrame(const Any& context) override;
    void DecodedBitStream(const Any& context) override;
    void EndFrame(const Any& context) override;
private:
    bool InitH264Picture(VAH264DecodePictureContext::ptr picture);
private:
    void OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue) override;
private:
    H264Deserialize::ptr                                            _deserialize;
    H264ContextSyntax::ptr                                          _deserializeContext;
    H264SliceDecodingProcess::ptr                                   _sliceDecoding;
    VAH264DecodePictureContext::ptr                                 _curPic;
};

} // namespace Codec
} // namespace Mmp