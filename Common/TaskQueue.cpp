#include "TaskQueue.h"

#include <cassert>
#include <mutex>
#include <unistd.h>

namespace Mmp 
{

TaskQueue::TaskQueue()
{
    _isRunning = false;
}

void TaskQueue::Start()
{
    assert(!_isRunning);
    _isRunning = true;
    _scheduler = std::make_shared<std::thread>([this]()
    {
        while (this->_isRunning) 
        {
            Task::ptr task = nullptr;
            {
                std::unique_lock<std::mutex> lock(this->_mtx);
                if (_tasks.empty())
                {
                    _conds.wait(lock);
                }
                if (_tasks.empty())
                {
                    continue;
                }
                task = _tasks.front();
                _tasks.pop_front();
            }
            task->Run();
        }
    });
}

void TaskQueue::Stop(bool sync)
{
    assert(_isRunning);
    if (sync)
    {
        std::deque<Promise<void>::ptr> tasks;
        {
            std::lock_guard<std::mutex> lock(this->_mtx);
            tasks = _tasks;
        }
        for (auto& task : tasks)
        {
            task->Wait();
        }
    }
    _isRunning = true;
    _conds.notify_one();
    _scheduler->join();
}

void TaskQueue::Commit(Promise<void>::ptr task)
{
    assert(_isRunning);
    std::lock_guard<std::mutex> lock(_mtx);
    _tasks.push_back(task);
    _conds.notify_one();
}

} // namespace Mmp 