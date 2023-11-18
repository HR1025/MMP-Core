//
// VACommon.h
//
// Library: Common
// Package: Codec
// Module:  VAAPI
// 

#pragma once

#include "Common/Common.h"

#include <va/va.h>
#if MMP_PLATFORM(LINUX)
#include <va/va_drm.h>
#endif
#ifdef USE_X11
#include <va/va_x11.h>
#endif /* USE_X11 */

#include "Codec/CodecCommon.h"
#include "Codec/AbstractEncoder.h"
#include "Codec/AbstractDecorder.h"

namespace Mmp
{
namespace Codec
{

} // namespace Codec
} // namespace Mmp