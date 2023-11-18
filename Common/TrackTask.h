//
// TrackTask.h
//
// Library: Thread
// Package: Task
// Module:  TrackTask
// 

#pragma once

#include <string>

#include "Task.h"

namespace Mmp
{
class TrackTaskImpl;
/**
 * @brief TrackTask 可用于跟踪任务,辅助异步调试 (主要以日志打印的形式)
 */
class TrackTask : public Task
{
public:
    explicit TrackTask(const TaskFunc& func, const std::string& tag = std::string());
    explicit TrackTask(Task::ptr task, const std::string& tag = std::string());
    ~TrackTask();
public:
    void Run() override;
    void SetPriority(const TaskPriority& priority) override;
    TaskPriority GetPriority() override;
    void SetType(const TaskType& type) override;
    TaskType GetType() override;
private:
    TrackTaskImpl* _impl;
};

} // namespace Mmp