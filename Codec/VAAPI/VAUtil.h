//
// VAUtil.h
//
// Library: Common
// Package: Codec
// Module:  VAAPI
// 

#pragma once

#include "VACommon.h"
#include "VaH264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

std::string VAProfileToStr(VAProfile profile);

VaDecodePictureContext::ptr VaH264DecodePictureContextToVaDecodePictureContext(VaH264DecodePictureContext::ptr context);

} // namespace Codec
} // namespace Mmp