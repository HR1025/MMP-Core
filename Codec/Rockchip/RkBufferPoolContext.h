//
// RkBufferPoolContext.h
//
// Library: Common
// Package: Codec
// Module:  Rockchip
// 
//

#pragma once

#include <unordered_map>

#include "RKCommon.h"
#include "RkBufferContext.h"

namespace Mmp
{
namespace Codec
{

class RkBufferPoolContext
{
public:
    using ptr = std::shared_ptr<RkBufferPoolContext>;
public:
    RkBufferPoolContext();
    ~RkBufferPoolContext();
public:
    void Init(size_t bufSize, uint32_t poolSize);
    MppBufferGroup GetFrameGroup();
    RkBufferContext::ptr GetRkBufferContextByFd(int fd);
private:
    std::mutex _mtx;
    MppBufferGroup _frameGrp;
    std::unordered_map<int /* (drm) fd */, RkBufferContext::ptr> _pool;
};

} // namespace Codec
} // namespace Mmp