//
// RPCProxy.h
//
// Library: RPC
// Package: RPC
// Module:  Factory
// 

#pragma once

#include "AbstractRPC.h"

#include <map>
#include <mutex>

namespace Mmp
{
namespace RPC
{

class RPCProxy : public AbstractRPC
{
public:
    using ptr = std::shared_ptr<RPCProxy>;
public:
    RPCProxy();
    ~RPCProxy();
public:
    static RPCProxy::ptr Instance();
public:
    bool Init() override;
    void Uninit() override;
    bool Start() override;
    void Stop() override;
public:
    void AddDelegate(const std::string& className);
    void DelDelegate(const std::string& className);
    AbstractRPC::ptr GetDelegate(const std::string& className);
private:
    std::mutex _mtx;
    bool       _isStarted;
    std::map<std::string /* className */, AbstractRPC::ptr> _delegates;
};

} // namespace RPC
} // namespace Mmp