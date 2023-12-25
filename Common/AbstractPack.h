//
// AbstractPack.h
//
// Library: Common
// Package: Memory
// Module:  Pack
// 

#pragma once

#include <memory>

#include "Any.h"
#include "AbstractSharedData.h"

namespace Mmp
{

/**
 * @brief 通用压缩数据定义
 */
class AbstractPack : public AbstractSharedData
{
public:
    using ptr = std::shared_ptr<AbstractPack>;
public:
    explicit AbstractPack();
public:
    Any sideData;
};

} // namespace Mmp