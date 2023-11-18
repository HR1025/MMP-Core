//
// AbstractRPCRequest.h
//
// Library: RPC
// Package: RPC
// Module:  Message
// 

#pragma once

#include "RPCCommon.h"

namespace Mmp
{
namespace RPC
{

class AbstractRPCDispatcher
{
public:
    using ptr = std::shared_ptr<AbstractRPCDispatcher>;
public:
    AbstractRPCDispatcher() = default;
    virtual ~AbstractRPCDispatcher() = default;
public:
    virtual bool Match(const std::string& strategy) = 0;
};

class SimpleRPCDispatcher : public AbstractRPCDispatcher
{
public:
    SimpleRPCDispatcher(const std::string strategy);
public:
    bool Match(const std::string& strategy) override;
private:
    std::string _strategy;
};


} // namespace RPC
} // namespace Mmp