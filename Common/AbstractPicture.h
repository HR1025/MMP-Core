//
// AbstractPicture.h
//
// Library: Common
// Package: Memory
// Module:  Picture
//

#pragma once

#include "AbstractFrame.h"
#include "PixelsInfo.h"

namespace Mmp
{

/**
 * @brief  通用图片定义
 */
class AbstractPicture : public AbstractFrame
{
public:
    using ptr = std::shared_ptr<AbstractPicture>;
public:
    explicit AbstractPicture(PixelsInfo info);
public:
    PixelsInfo info;
};

} // namespace Mmp