#include "RPCFactory.h"

#include <mutex>
#include <map>

#include "Common/FuncTask.h"
#include "Common/ThreadPool.h"

namespace Mmp
{
namespace RPC
{

class AbstractRPC::Impl
{
public:
    std::mutex                                       _mtx;
    std::map<void*, AbstractRPCDispatcher::ptr>      _dispatchers;
    std::map<void*, RPCTask>                         _tasks;
};

AbstractRPC::AbstractRPC()
    : _impl(std::make_shared<Impl>())
{

}

void AbstractRPC::Register(AbstractRPCDispatcher::ptr dispatcher, RPCTask task)
{
    std::lock_guard<std::mutex> lock(_impl->_mtx);
    _impl->_dispatchers[dispatcher.get()] = dispatcher;
    _impl->_tasks[dispatcher.get()] = task;
}

void AbstractRPC::Unregister(AbstractRPCDispatcher::ptr dispatcher)
{
    std::lock_guard<std::mutex> lock(_impl->_mtx);
    _impl->_dispatchers.erase(dispatcher.get());
    _impl->_tasks.erase(dispatcher.get());
}

void AbstractRPC::DoRequest(Any user, const std::string& strategy, AbstractSharedData::ptr request, bool sync)
{
    auto task = std::make_shared<FuncTask>([this, user, strategy, request]
    {
        std::map<void*, AbstractRPCDispatcher::ptr> dispatchers; // Hint : just use to keep alive
        std::map<void*, RPCTask> tasks;
        {
            std::lock_guard<std::mutex> lock(this->_impl->_mtx);
            tasks = this->_impl->_tasks;
            dispatchers = this->_impl->_dispatchers;
        }
        bool found = false;
        for (const auto& tmp : tasks)
        {
            AbstractRPCDispatcher* dispatcher = (AbstractRPCDispatcher *)tmp.first;
            RPCTask task = tmp.second;
            if (dispatcher->Match(strategy))
            {
                Any _request;
                Any _response;
                AbstractSharedData::ptr response; 
                if (!task.rpcRequest->Deserialize(request, _request))
                {
                    break;
                }
                if (!task.rpcProcess->Process(_request, _response))
                {
                    break;
                }
                if (!task.rpcResponse->Serialize(_response, response))
                {
                    break;
                }
                this->OnResponse(user, response);
                found = true;
            }
        }
        if (!found)
        {
            this->OnNotFoundDispatcher(user, strategy);
        }
    });
    if (!sync)
    {
        ThreadPool::ThreadPoolSingleton()->Commit(task);
    }
    else
    {
        task->Run();
    }
}

void AbstractRPC::OnResponse(Any /* user */, AbstractSharedData::ptr /* response */)
{

}

void AbstractRPC::OnNotFoundDispatcher(Any /* user */, const std::string& /* strategy */)
{

}

} // namespace RPC
} // namespace Mmp