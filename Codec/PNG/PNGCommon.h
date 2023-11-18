//
// AbstractEncoder.h
//
// Library: Common
// Package: Codec
// Module:  Png
// 

#pragma once

#include "Codec/CodecCommon.h"
#include "Codec/AbstractEncoder.h"
#include "Codec/AbstractDecorder.h"

namespace Mmp
{
namespace Codec
{
struct PNGEncoderParameter
{

};

struct PngDecoderParameter
{
    PixelFormat format;
};
} // namespace Codec
} // namespace Mmp