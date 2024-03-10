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

std::string VAStatusToStr(VAStatus status);

} // namespace Codec
} // namespace Mmp