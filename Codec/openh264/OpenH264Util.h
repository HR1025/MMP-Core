//
// OpenH264Util.h
//
// Library: Codec
// Package: Codec
// Module:  OpenH264
// 

#pragma once

#include "OpenH264Common.h"

namespace Mmp
{
namespace Codec
{

void OpenH264LogCallBack(void *ctx, int level, const char *msg);

PixelFormat OpenH264PixFormatToMppPixFormat(EVideoFormatType type);

} // namespace Codec
} // namespace Mmp