//
// NormalPicture.h
//
// Library: Common
// Package: Memory
// Module:  Picture
//

#pragma once

#include "AbstractPicture.h"
#include "SharedDataDecorator.h"

namespace Mmp
{

/**
 * @brief 默认图片实现 
 */
class NormalPicture : public SharedDataDecoratorTemplate<AbstractPicture>
{
public:
    using ptr = std::shared_ptr<NormalPicture>;
public:
    explicit NormalPicture(PixelsInfo info, AbstractAllocateMethod::ptr allocateMethod = nullptr);
};

} // namespace Mmp