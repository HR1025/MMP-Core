//
// PixelsInfo.h
//
// Library: Common
// Package: Media
// Module:  Description
// 

#pragma once

#include <cstdint>

#include "PixelFormat.h"

namespace Mmp
{

/**
 * @brief 像素信息
 */
struct PixelsInfo
{
public:
    PixelsInfo();
    PixelsInfo(int32_t width, int32_t height, int32_t bitdepth, PixelFormat format);
public:
    int32_t     width;
    int32_t     height;
    int32_t     bitdepth;
    PixelFormat format;
};

extern bool operator==(const PixelsInfo& left, const PixelsInfo& right);
extern bool operator!=(const PixelsInfo& left, const PixelsInfo& right);

}; // namespace Mmp