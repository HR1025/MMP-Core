//
// NormalFrame.h
//
// Library: Common
// Package: Memory
// Module:  Frame
// 

#pragma once

#include "AbstractFrame.h"
#include "SharedDataDecorator.h"

namespace Mmp
{

/**
 * @brief 默认原生数据帧实现 
 */
class NormalFrame : public SharedDataDecoratorTemplate<AbstractFrame>
{
public:
    using ptr = std::shared_ptr<NormalFrame>();
public:
    explicit NormalFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
};

} // namespace Mmp