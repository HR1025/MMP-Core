#include "RkBufferPoolContext.h"

#include <cassert>

#include "RKUtil.h"

namespace Mmp
{
namespace Codec
{

RkBufferPoolContext::RkBufferPoolContext()
{
    MPP_RET rkRet = MPP_RET::MPP_OK;
    if (RK_OP_FAIL(mpp_buffer_group_get_external(&_frameGrp, MPP_BUFFER_TYPE_DMA_HEAP)))
    {
        RK_LOG_ERROR << "mpp_buffer_group_get_external fail, error is: " << RkMppRetToStr(rkRet);
        assert(false);
    }
}

RkBufferPoolContext::~RkBufferPoolContext()
{
    if (_frameGrp)
    {
        mpp_buffer_group_put(_frameGrp);
        _frameGrp = nullptr;
    }
}

void RkBufferPoolContext::Init(size_t bufSize, uint32_t poolSize)
{
    std::lock_guard<std::mutex> lock(_mtx);
    MPP_RET rkRet = MPP_RET::MPP_OK;
    MppBufferInfo commit = {};
    {
        commit.type = MppBufferType::MPP_BUFFER_TYPE_DMA_HEAP;
        commit.size = bufSize;
    }
    for (uint32_t i=0; i<poolSize; i++)
    {
        RkBufferContext::ptr buf = std::make_shared<RkBufferContext>();
        buf->Malloc(bufSize);
        _pool[buf->GetFd()] = buf;
        commit.fd = buf->GetFd();
        commit.index = commit.fd;
        if (RK_OP_FAIL(mpp_buffer_commit(_frameGrp, &commit)))
        {
            RK_LOG_ERROR << "mpp_buffer_commit fail, error is: " << RkMppRetToStr(rkRet);
            assert(false);
        }
    }
}

MppBufferGroup RkBufferPoolContext::GetFrameGroup()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _frameGrp;
}

RkBufferContext::ptr RkBufferPoolContext::GetRkBufferContextByFd(int fd)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_pool.count(fd))
    {
        return _pool[fd];
    }
    else
    {
        // assert(false);
        return nullptr;
    }
}

} // namespace Codec
} // namespace Mmp