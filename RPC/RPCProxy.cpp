#include "RPCProxy.h"

#include "RPCFactory.h"

namespace Mmp
{
namespace RPC
{
    
RPCProxy::RPCProxy()
{
    _isStarted = false;
}

RPCProxy::~RPCProxy()
{
    if (_isStarted)
    {
        for (const auto& delegate : _delegates)
        {
            delegate.second->Stop();
            delegate.second->Uninit();
        }
    }
}

RPCProxy::ptr RPCProxy::Instance()
{
    static RPCProxy::ptr instance = std::make_shared<RPCProxy>();
    return instance;
}

bool RPCProxy::Init()
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (const auto& delegate : _delegates)
    {
        delegate.second->Init();
    }
    return true;
}

void RPCProxy::Uninit()
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (const auto& delegate : _delegates)
    {
        delegate.second->Uninit();
    }
}

bool RPCProxy::Start()
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (const auto& delegate : _delegates)
    {
        delegate.second->Start();
    }
    _isStarted = true;
    return true;
}

void RPCProxy::Stop()
{
    std::lock_guard<std::mutex> lock(_mtx);
    for (const auto& delegate : _delegates)
    {
        delegate.second->Stop();
    }
    _isStarted = false;
}

void RPCProxy::AddDelegate(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_delegates.count(className) == 0)
    {
        auto delegate = RPCFactory::DefaultFactory().createRPC(className);
        if (delegate)
        {
            _delegates[className] = delegate;
            if (_isStarted)
            {
                delegate->Init();
                delegate->Start();
            }
        }
    }
}

void RPCProxy::DelDelegate(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_delegates.count(className))
    {
        if (_isStarted)
        {
            _delegates[className]->Stop();
            _delegates[className]->Uninit();
        }
        _delegates.erase(className);
    }
}

AbstractRPC::ptr RPCProxy::GetDelegate(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_delegates.count(className))
    {
        return _delegates[className];
    }
    else
    {
        return nullptr;
    }
}

} // namespace RPC
} // namespace Mmp