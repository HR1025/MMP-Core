//
// NormalFrame.h
//
// Library: Common
// Package: Memory
// Module:  Frame
// 

#pragma once

#include "AbstractFrame.h"

namespace Mmp
{

/**
 * @brief 默认原生数据帧实现 
 */
class NormalFrame : public AbstractFrame
{
public:
    using ptr = std::shared_ptr<NormalFrame>();
public:
    explicit NormalFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    ~NormalFrame();
public:
    void     SetCapacity(size_t size) override;
    size_t   GetCapcaity() override;
    void     SetSize(size_t size) override;
    size_t   GetSize() override;
    void*    GetData(uint64_t offset = 0) override;
    AbstractAllocateMethod::ptr GetAllocateMethod() override;
private:
    AbstractFrame::ptr _data;
};

} // namespace Mmp