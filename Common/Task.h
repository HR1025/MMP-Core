//
// Task.h
//
// Library: Thread
// Package: Task
// Module:  Task
// 

#pragma once

#include <memory>
#include <functional>

namespace Mmp
{

/**
 * @brief 任务优先级
 */
enum class TaskPriority
{
    HIGH,
    NORMAL,
    LOW
};

/**
 * @brief 任务类型
 * @note  DEDICATE 类型任务会单独新开线程执行,具有较高的系统资源开销
 */
enum class TaskType
{
    NORMAL,
    COMPUTE,
    IOBLOCKING,
    DEDICATE
};

using TaskFunc = std::function<void()>;

/**
 * @brief 任务单元
 * @note  合理设置 TaskPriority 和 TaskType 能帮助线程池优化任务调度
 */
class Task
{
public:
    using ptr = std::shared_ptr<Task>;
public:
    Task();
    virtual ~Task() = default;
public:
    /**
     * @brief 执行任务
     */
    virtual void Run() = 0;
    /**
     * @brief      设置任务优先级
     * @param[in]  priority
     */
    virtual void SetPriority(const TaskPriority& priority);
    /**
     * @brief     获取任务优先级
     */
    virtual TaskPriority GetPriority();
    /**
     * @brief       设置任务类型
     * @param[in]   type
     */
    virtual void SetType(const TaskType& type);
    /**
     * @brief       获取任务类型
     */
    virtual TaskType GetType();
private:
    TaskPriority  _priority;
    TaskType      _type;
};

} // namespace Mmp