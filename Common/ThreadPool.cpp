#include "ThreadPool.h"

#include <deque>
#include <mutex>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <cassert>
#include <condition_variable>

#include <Poco/Environment.h>
#include <Poco/SingletonHolder.h>

#include "LogMessage.h"

namespace Mmp
{

/**
 * @brief 根据任务类型计算其权重
 * @todo  可能以一个更好的方式进行
 * @note  权重影响线程的调度,以及整个线程池的负载均衡
 * @sa    MMP 的权重值采用 斐波那契数列 进行模拟,用于保证权重的非线性梯度以及一定程度的随机性
 *        斐波那契数列示例: 1，1，2，3，5，8，13，21，34，55，89，144
 */
static uint64_t TaskTypeToScore(TaskType task)
{
    switch (task)
    {
        case TaskType::NORMAL: return 3;
        case TaskType::IOBLOCKING: return 13;
        case TaskType::COMPUTE: return 55;
        case TaskType::DEDICATE:
        default:
            assert(false);
            return 1;
    }
};

class ThreadContext
{
public:
    using ptr = std::shared_ptr<ThreadContext>;
public:
    ThreadContext()
    {
        taskSize = 0;
        score    = 0;
        runing   = false;
        thread   = nullptr;
    }
    ~ThreadContext()
    {
        if (thread)
        {
            delete thread;
        }
    }
public:
    void Push(Task::ptr task, bool isFront = false);
    Task::ptr Pop();
    void Start();
    void Stop();
    void Sync();
public:
    std::thread*              thread;
    std::atomic<uint64_t>     taskSize;
    std::deque<Task::ptr>     taskQueue;
    std::mutex                mtx;
    std::condition_variable   cond;
    std::atomic<uint64_t>     score;
    std::atomic<bool>         runing;
    std::string               name;
};

static void WrokThreadFunc(ThreadContext* context)
{
    MMP_LOG_INFO << "MMP thread pool begin, thread name is: " << context->name;
    Task::ptr task;
    do
    {
        task = context->Pop();
        if (task)
        {
            task->Run();
        }
    } while (task);
    MMP_LOG_INFO << "MMP thread pool end, thread name is: " << context->name;
}

void ThreadContext::Push(Task::ptr task, bool isFront)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (!runing) return;
    score += TaskTypeToScore(task->GetType());
    if (isFront)
    {
        taskQueue.push_front(task);
    }
    else
    {
        taskQueue.push_back(task);
    }
    taskSize++;
    cond.notify_one();
}

Task::ptr ThreadContext::Pop()
{
    std::unique_lock<std::mutex> lock(mtx);
    if (runing && taskSize == 0)
    {
        cond.wait(lock);
    }
    if (!runing)
    {
        return nullptr;
    }
    Task::ptr task = taskQueue.front();
    taskQueue.pop_front();
    taskSize--;
    score -= TaskTypeToScore(task->GetType());
    return task;
}

void ThreadContext::Start()
{
    runing = true;
    thread = new std::thread(&WrokThreadFunc, this);
}

void ThreadContext::Stop()
{
    runing = false;
    std::unique_lock<std::mutex> lock(mtx);
    cond.notify_one();
}

void ThreadContext::Sync()
{
    if (thread->joinable())
    {
        thread->join();
    }
}

class ThreadPoolImpl : public ThreadPool
{
public:
    ThreadPoolImpl() = default;
public:
    void Init(uint32_t threadNum) override;
    void Uninit() override;
    uint32_t PoolSize() override;
    void Commit(Task::ptr task) override;
    void Commit(Task::ptr task, uint32_t slot) override;
private:
    std::vector<ThreadContext::ptr>  _threads;
    uint32_t                         _threadNum; 
};

void ThreadPoolImpl::Init(uint32_t threadNum)
{
    _threadNum = threadNum ? threadNum : (uint32_t)Poco::Environment::processorCount(); 
    for (uint32_t i = 0; i<_threadNum; i++)
    {
        ThreadContext::ptr context = std::make_shared<ThreadContext>();
        _threads.push_back(context);
    }
    for (uint32_t i = 0; i<_threadNum; i++)
    {
        _threads[i]->name = "MMP_ThreadPool_" + std::to_string(i);
        _threads[i]->Start();
    }
}

void ThreadPoolImpl::Uninit()
{
    // Hint : not to modify _threads, destroy when deconstruct (for not lock purpose)
    for (uint32_t i = 0; i<_threadNum; i++)
    {
        _threads[i]->Stop();
    }
    for (uint32_t i = 0; i<_threadNum; i++)
    {
        _threads[i]->Sync();
    }
}

uint32_t ThreadPoolImpl::PoolSize()
{
    return _threadNum;
}

static void RunTask(Task::ptr task)
{
    task->Run();
}

void ThreadPoolImpl::Commit(Task::ptr task)
{
    if (task->GetType() == TaskType::DEDICATE)
    {
        std::thread(&RunTask, task).detach();
    }
    else
    // TODO : 完善负载均衡策略 (???)
    // 1 - 区分中、低优先级任务
    // 2 - 实现不同线程间任务迁移,避免饥饿现象 (由于某个超长耗时任务)
    // 3 - 权值计算规则
    // 4 - 任务类型合并 - 例如让 CPU 、IO 密集型任务尽可能执行在固定线程上,方便内核进行调度
    // Hint : 目前 MMP 好像似乎还没有这么复杂的场景,当前负载均衡策略应当可以满足眼下场景
    {
        uint64_t score = _threads[0]->score;
        uint32_t slot = 0;
        for (uint32_t i=1; i<_threadNum; i++)
        {
            if (score > _threads[i]->score)
            {
                score = _threads[i]->score;
                slot = i;
            }
            if (score == 0) /* full ideal */
            {
                break;
            }
        }
        if (task->GetPriority() == TaskPriority::HIGH)
        {
            _threads[slot]->Push(task, true);
        }
        else
        {
            _threads[slot]->Push(task, false);
        }
    }
}

void ThreadPoolImpl::Commit(Task::ptr task, uint32_t slot)
{
    // Hint : 按 slot 投递任务时禁用内部负载均衡策略
    if (task->GetType() == TaskType::DEDICATE)
    {
        assert(false);
        task->SetType(TaskType::NORMAL);
    }
    _threads[slot%_threadNum]->Push(task);
}

static Poco::SingletonHolder<ThreadPoolImpl> sh;

ThreadPool* ThreadPool::ThreadPoolSingleton()
{
    return sh.get();
}

} // namespace Mmp