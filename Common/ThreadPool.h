//
// ThreadPool.h
//
// Library: Thread
// Package: Thread
// Module:  ThreadPool
// 

#pragma once

#include <memory>
#include <cstdint>

#include "Task.h"

namespace Mmp
{

/**
 * @brief 公共线程池
 */
class ThreadPool
{
public:
    ThreadPool() = default;
    ~ThreadPool() = default;
public:
    /**
     * @brief       初始化线程池
     * @param[in]   threadNum : 线程池线程数量
     * @note        需在 Commit 前进行调用,否则内部自行初始化,默认线程数为 CPU 核心数
     */
    virtual void Init(uint32_t threadNum = 0) = 0;
    /**
     * @brief       重置线程池
     */
    virtual void Uninit() = 0;
    /**
     * @brief       线程池大小
     */
    virtual uint32_t PoolSize() = 0;
    /**
     * @brief       提交任务
     * @param[in]   task
     * @note        线程池内部实现负载均衡,保证任务尽快得到调度
     */
    virtual void Commit(Task::ptr task) = 0;
    /**
     * @brief       提交任务
     * @param[in]   task
     * @param[in]   slot
     * @note        相同的 slot 将会被投递到同一个线程 (保证串行执行)
     */
    virtual void Commit(Task::ptr task, uint32_t slot) = 0;
public:
    static ThreadPool* ThreadPoolSingleton();
};

} // namespace Mpp