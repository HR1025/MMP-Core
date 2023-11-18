//
// MediaType.h
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
 * @brief 媒体类型
 */
enum class MeidaType
{
    AUDIO,
    VIDEO,
};
const std::string MediaTypeToStr(MeidaType type);
extern std::ostream& operator<<(std::ostream& os, MeidaType type);

} // namespace Mmp