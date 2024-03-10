//
// VADevice.h
//
// Library: Common
// Package: Codec
// Module:  VAAPI
// 

#pragma once

#include <mutex>
#include <vector>
#include <memory>

#include "VACommon.h"

namespace Mmp
{
namespace Codec
{

VAProfile H264ProfileToVaProfile(H264Profile profile);

uint32_t H264ChromaFormatToVaChromaFormat(H264ChromaFormat format);

uint32_t H264SliceTypeToVaSliceType(H264SliceType type);

uint32_t PixelFormatToVaRTFormat(PixelFormat format);

uint32_t PixelFormatToVaFourcc(PixelFormat format);

} // namespace Codec
} // namespace Mmp