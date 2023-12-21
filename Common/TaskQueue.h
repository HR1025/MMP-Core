//
// TaskQueue.h
//
// Library: Thread
// Package: Thread
// Module:  TaskQueue
// 

#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>
#include <cstdint>
#include <condition_variable>

#include "Promise.h"

namespace Mmp 
{

/**
 * @brief 任务队列
 */
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue() = default;
public:
    /**
     * @brief       开始调度
     */
    void Start();
    /**
     * @brief       停止调度
     * @param[in]   当 sync 为 true 时, 将会强制等待所有未处理的任务执行完成
     */
    void Stop(bool sync = true);
    /**
     * @brief       提交任务
     * @param[in]   task
     */
    void Commit(Promise<void>::ptr task);
private:
    std::mutex _mtx;
    std::atomic<bool> _isRunning;
    std::condition_variable _conds;
    std::deque<Promise<void>::ptr> _tasks;
    std::shared_ptr<std::thread> _scheduler;
};

} // namespace Mmp 

