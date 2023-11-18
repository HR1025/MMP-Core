//
// AbstractFrame.h
//
// Library: Common
// Package: Memory
// Module:  Frame
// 

#pragma once

#include <memory>

#include "Any.h"
#include "SharedData.h"
#include "PixelsInfo.h"

namespace Mmp
{

/**
 * @brief 通用原生数据帧描述
 */
class AbstractFrame : public SharedData
{
public:
    using ptr = std::shared_ptr<AbstractFrame>;
public:
    explicit AbstractFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
public:
    Any sideData;
};


class PictureFrame : public AbstractFrame
{
public:
    using ptr = std::shared_ptr<PictureFrame>;
public:
    explicit PictureFrame(PixelsInfo info, AbstractAllocateMethod::ptr allocateMethod = nullptr);
public:
    PixelsInfo info;
};

} // namespace Mmp