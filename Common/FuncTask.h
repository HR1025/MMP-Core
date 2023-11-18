//
// FuncTask.h
//
// Library: Thread
// Package: Task
// Module:  FuncTask
// 

#pragma once

#include "Task.h"

namespace Mmp
{

/**
 * @note 实用类,将 lambda 包装成 FuncTask
 */
class FuncTask : public Task
{
public:
    explicit FuncTask(const TaskFunc& func);
public:
    void Run() override;
private:
    TaskFunc _func;
};

} // namespace Mmp