//
// RPCFactory.h
//
// Library: RPC
// Package: RPC
// Module:  Factory
// 

#pragma once

#include "Common/Instantiator.h"
#include "Common/DynamicFactory.h"
#include "RPCCommon.h"
#include "AbstractRPC.h"

namespace Mmp
{
namespace RPC
{

/**
 * @brief RPC工厂
 */
class RPCFactory
{
public:
    using AbstractRPCInstantiator = AbstractInstantiator<AbstractRPC>;
public:
    RPCFactory();
    ~RPCFactory();
    /**
     * @brief      注册RPC创建器到工厂
     * @param[in]  className
     * @param[in]  instantiator
     * @note       AbstractRPCInstantiator 需通过 new 的方式进行创建
     */
    void registerClass(const std::string& className, AbstractRPCInstantiator* instantiator);
    /**
     * @brief 创建RPC
     */
    AbstractRPC::ptr createRPC(const std::string& className);
    /**
     * @brief 获取RPC工厂默认单例
     */
    static RPCFactory& DefaultFactory();
private:
    void RegisterBuiltins();
private:
    std::mutex _mutex;
    DynamicFactory<AbstractRPC> _rpcFactory;
};

} // namespace RPC
} // namespace Mmp