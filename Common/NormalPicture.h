//
// NormalPicture.h
//
// Library: Common
// Package: Memory
// Module:  Picture
//

#pragma once

#include "AbstractPicture.h"

namespace Mmp
{

/**
 * @brief 默认图片实现 
 */
class NormalPicture : public AbstractPicture
{
public:
    using ptr = std::shared_ptr<NormalPicture>;
public:
    explicit NormalPicture(PixelsInfo info, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    ~NormalPicture();
public:
    void     SetCapacity(size_t size) override;
    size_t   GetCapcaity() override;
    void     SetSize(size_t size) override;
    size_t   GetSize() override;
    void*    GetData(uint64_t offset = 0) override;
    AbstractAllocateMethod::ptr GetAllocateMethod() override;
private:
    void*  _data;
    size_t _capacity;
    size_t _size;
private:
    AbstractAllocateMethod::ptr _allocateMethod;
};

} // namespace Mmp