//
// RKTranslator.h
//
// Library: Common
// Package: Codec
// Module:  Rockchip
// 

#pragma once

#include "RKCommon.h"

namespace Mmp
{
namespace Codec
{

MppCodingType CodecTypeToRkType(CodecType type);

PixelFormat MppFrameFormatToPixformat(MppFrameFormat format);

} // namespace Codec
} // namespace Mmp