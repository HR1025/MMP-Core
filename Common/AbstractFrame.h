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
#include "AbstractSharedData.h"

namespace Mmp
{

/**
 * @brief 通用原生数据帧定义
 */
class AbstractFrame : public AbstractSharedData
{
public:
    using ptr = std::shared_ptr<AbstractFrame>;
public:
    explicit AbstractFrame();
public:
    Any sideData;
};

} // namespace Mmp