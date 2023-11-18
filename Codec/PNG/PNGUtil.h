//
// AbstractEncoder.h
//
// Library: Common
// Package: Codec
// Module:  Png
// 

#pragma once

#include <cstdint>
#include <string>

#include "Common/AbstractAllocateMethod.h"

namespace Mmp
{
namespace Codec
{

/**
 * @note lodepng memory management wrapper
 * @sa   lodepng_decode_memory 、lodepng_encode_memory
 */
class PngAllocateMethod : public AbstractAllocateMethod
{
public:
    static void Register();
public:
    PngAllocateMethod();
    ~PngAllocateMethod();
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public:
    void*       data;
    size_t      size;
private:
    std::string _tag;
};

} // namespace Codec
} // namespace Mmp