//
// AbstractRPCProcess.h
//
// Library: RPC
// Package: RPC
// Module:  Process
// 

#pragma once

#include "RPCCommon.h"

namespace Mmp
{
namespace RPC
{

/**
 * @brief RPC 任务处理
 */
class AbstractRPCProcess
{
public:
    using ptr = std::shared_ptr<AbstractRPCProcess>;
public:
    AbstractRPCProcess() = default;
    virtual ~AbstractRPCProcess() = default;
public:
    /**
     * @brief      任务处理
     * @param[in]  request
     * @param[in]  response
     */
    virtual bool Process(const Any& request, Any& response) = 0;
};

class FuncRPCProcess : public AbstractRPCProcess
{
public:
    explicit FuncRPCProcess(RPCProcess process);
public:
    bool Process(const Any& request, Any& response) override;
private:
    RPCProcess _process;
};

} // namespace RPC
} // namespace Mmp