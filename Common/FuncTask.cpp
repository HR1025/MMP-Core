#include "FuncTask.h"

namespace Mmp
{

FuncTask::FuncTask(const TaskFunc& func)
{
    _func = func;
}

void FuncTask::Run()
{
    _func();
}

} // namespace Mmp