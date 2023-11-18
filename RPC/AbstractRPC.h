//
// AbstractRPC.h
//
// Library: RPC
// Package: RPC
// Module:  RPC
// 

#pragma once

#include "RPCCommon.h"
#include "AbstractRPCDispatcher.h"
#include "AbstractRPCRequest.h"
#include "AbstractRPCResponse.h"
#include "AbstractRPCProcess.h"

namespace Mmp
{
namespace RPC
{

class RPCTask
{
public:
    AbstractRPCRequest::ptr   rpcRequest;
    AbstractRPCResponse::ptr  rpcResponse;
    AbstractRPCProcess::ptr   rpcProcess;
};

/**
 * @brief 通用RPC接口定义
 */
class AbstractRPC
{
public:
    using ptr = std::shared_ptr<AbstractRPC>;
public:
    AbstractRPC();
    virtual ~AbstractRPC() = default;
public:
    /**
     * @brief 初始化
     */
    virtual bool Init() = 0;
    /**
     * @brief 重置
     */
    virtual void Uninit() = 0;
    /**
     * @brief 开始运行
     */
    virtual bool Start() = 0;
    /**
     * @brief 结束运行
     */
    virtual void Stop() = 0;
public:
    /**
     * @brief       注册任务处理回调钩子
     * @param[in]   dispatcher
     * @param[in]   task
     */
    void Register(AbstractRPCDispatcher::ptr dispatcher, RPCTask task);
    /**
     * @brief       注销任务处理回调钩子
     * @param[in]   dispatcher
     */
    void Unregister(AbstractRPCDispatcher::ptr dispatcher);
protected:
    /**
     * @brief      处理请求
     * @param[in]  user : 自定义数据
     * @param[in]  strategy : 请求策略
     * @param[in]  request : 请求数据
     * @param[in]  sync
     */
    void DoRequest(Any user, const std::string& strategy, SharedData::ptr request, bool sync = false);
    /**
     * @brief      处理结果事件回调
     * @param[in]  user : 自定义数据
     * @param[in]  response : 处理结果
     */
    virtual void OnResponse(Any user, SharedData::ptr response);
    /**
     * @brief      未找到事件处理任务回调
     * @param[in]  user : 自定义数据
     * @param[in]  strategy : 请求策略
     */
    virtual void OnNotFoundDispatcher(Any user, const std::string& strategy);
private:
    class Impl;
    std::shared_ptr<Impl> _impl;
};

} // namespace RPC
} // namespace Mmp