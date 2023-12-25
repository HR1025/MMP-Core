//
// AbstractRPCResponse.h
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

/**
 * @brief RPC 执行结果
 */
class AbstractRPCResponse
{
public:
    using ptr = std::shared_ptr<AbstractRPCResponse>;
public:
    AbstractRPCResponse() = default;
    virtual ~AbstractRPCResponse() = default;
public:
    /**
     * @brief 序列化,将 C/C++ 数据结构序列化为原生数据
     */
    virtual bool Serialize(const Any& form, AbstractSharedData::ptr& to) = 0;
};

class FuncRPCResponse : public AbstractRPCResponse
{
public:
    explicit FuncRPCResponse(RPCSerialize serialize);
public:
    bool Serialize(const Any& form, AbstractSharedData::ptr& to) override;
private:
    RPCSerialize _serialize;
};

} // namespace RPC
} // namespace Mmp