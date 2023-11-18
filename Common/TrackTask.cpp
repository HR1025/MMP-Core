#include "TrackTask.h"

#include <mutex>
#include <string>
#include <cassert>

#include <Poco/Stopwatch.h>

#include "FuncTask.h"
#include "LogMessage.h"

namespace Mmp
{

/**
 * @note trackId 需要确保唯一性,传统做法可以考虑使用随机数生成来实现;
 *       但往往随机数生成是耗时耗资源操作,在 TrackTaskImpl 的场景下,
 *       我们只需要保证 trackId 的唯一性,而不需要考虑其随机性,
 *       故 atmoic ++ 可能是一种不错的解决方案
 */
std::string GenerateTrackId()
{
    static std::atomic<uint64_t> random(0xFF66CCFF);
    return std::to_string(random++);
};

class TrackTaskImpl
{
public:
    explicit TrackTaskImpl(Task::ptr task, const std::string& tag);
public:
    void Run();
public:
    Task::ptr _task;
private:
    Poco::Stopwatch  _stopWatch;
private:
    std::string      _tag;
    std::string      _trackId;
    uint64_t         _pendingTimeMs;
    uint64_t         _executeTimeMs;
};

TrackTaskImpl::TrackTaskImpl(Task::ptr task, const std::string& tag)
{
    _task = task;
    _trackId = GenerateTrackId();
    if (!tag.empty())
    {
        _trackId += std::string() + "(" + tag + ")";
    }
    _stopWatch.start();
    MMP_LOG_INFO << "Track task create, track id is: " << _trackId;
}

void TrackTaskImpl::Run()
{
    _pendingTimeMs = _stopWatch.elapsed() / 1000;
    MMP_LOG_INFO << "Track task begin, track id is : " << _trackId;
    _task->Run();
    _executeTimeMs = _stopWatch.elapsed() / 1000 - _pendingTimeMs;
    MMP_LOG_INFO << "Track task end, track id is: " << _trackId << ","
                 << "pending time is: " << _pendingTimeMs << " ms,"
                 << "execute time is: " << _executeTimeMs << " ms,"
                 << "total time is: " << (_pendingTimeMs + _executeTimeMs) << " ms";
}

TrackTask::TrackTask(const TaskFunc& func, const std::string& tag)
    : TrackTask(std::make_shared<FuncTask>(func), tag)
{
}

TrackTask::TrackTask(Task::ptr task, const std::string& tag)
    : _impl(new TrackTaskImpl(task, tag))
{
    assert(_impl);
}

TrackTask::~TrackTask()
{
    delete _impl;
}

void TrackTask::Run()
{
    _impl->Run();
}

void TrackTask::SetPriority(const TaskPriority& priority)
{
    _impl->_task->SetPriority(priority);
}

TaskPriority TrackTask::GetPriority()
{
    return _impl->_task->GetPriority();
}

void TrackTask::SetType(const TaskType& type)
{
    _impl->_task->SetType(type);
}

TaskType TrackTask::GetType()
{
    return _impl->_task->GetType();
}

} // namespace Mmp