#include "Task.h"

namespace Mmp
{

Task::Task()
{
    _priority = TaskPriority::NORMAL;
    _type     = TaskType::NORMAL;
}

void Task::SetPriority(const TaskPriority& priority)
{
    _priority = priority;
}

TaskPriority Task::GetPriority()
{
    return _priority;
}

void Task::SetType(const TaskType& type)
{
    _type = type;
}

TaskType Task::GetType()
{
    return _type;
}

} // namespace Mmp