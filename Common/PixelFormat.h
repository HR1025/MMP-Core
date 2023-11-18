//
// PixelFormat.h
//
// Library: Common
// Package: Media
// Module:  Format
// 

#pragma once

#include <iostream>
#include <string>

namespace Mmp
{

/**
 * @brief  像素格式
 * @note   1 - RGB为小端格式
 *         2 - YUV 类型只支持 8 bit
 */
enum class PixelFormat
{
    YUV420P,     /* YYYY... UUUU... VVVV*/
    NV12,        /* YYYY... UV...*/ 
    RGB565,
    ARGB1555,
    RGBA5551,
    RGB888,
    ARGB8888,
    RGBA8888,
    BGR565,
    ABGR1555,
    BRGA5551,
    BGR888,
    BGRA8888,
    ABGR8888,
    HSV888,
};
float BytePerPixel(PixelFormat format);

const std::string PixelFormatToStr(PixelFormat format);
extern std::ostream& operator<<(std::ostream& os, PixelFormat format);

} // namespace Mmp