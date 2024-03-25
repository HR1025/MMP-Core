//
// RKH264Decoder.h
//
// Library: Common
// Package: Codec
// Module:  Rockchip
// 
//

#pragma once

#include "RKDecoder.h"

namespace Mmp
{
namespace Codec
{

class RKH264Decoder : public RKDecoder
{
public:
    CodecType GetCodecType() override;
};

} // namespace Codec
} // namespace Mmp