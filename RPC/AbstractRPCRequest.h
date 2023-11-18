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

/**
 * @brief RPC 任务请求
 */
class AbstractRPCRequest
{
public:
    using ptr = std::shared_ptr<AbstractRPCRequest>;
public:
    AbstractRPCRequest() = default;
    virtual ~AbstractRPCRequest() = default;
public:
    /**
     * @brief       反序列化,将原生数据反序列化为 C/C++ 数据结构
     * @param[in]   from
     * @param[in]   to
     */
    virtual bool Deserialize(SharedData::ptr from, Any& to) = 0;
};

class FuncRPCRequest : public AbstractRPCRequest
{
public:
    explicit FuncRPCRequest(RPCDeserialize deserialize);
public:
    bool Deserialize(SharedData::ptr from, Any& to) override;
private:
    RPCDeserialize _deserialize;
};

} // namespace RPC
} // namespace Mmp