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
    bool Push(Pack::ptr pack) override;
private:
    void StartFrame(const Any& context) override;
    void DecodedBitStream(const Any& context) override;
    void EndFrame(const Any& context) override;
private:
    void OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue) override;
private:
    VaDecodePictureContext _picContext;
    H264Deserialize::ptr   _deserialize;
};

} // namespace Codec
} // namespace Mmp