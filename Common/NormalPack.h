//
// NormalPack.h
//
// Library: Common
// Package: Memory
// Module:  Pack
// 

#pragma once

#include "AbstractPack.h"
#include "SharedDataDecorator.h"

namespace Mmp
{

/**
 * @brief 默认压缩数据实现 
 */
class NormalPack : public SharedDataDecoratorTemplate<AbstractPack>
{
public:
    using ptr = std::shared_ptr<NormalPack>;
public:
    explicit NormalPack(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
};

} // namespace Mmp