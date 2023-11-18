#include "RPCFactory.h"

#ifdef RPC_WITH_HTTP
#include "HTTP/HTTPRPC.h"
#endif /* RPC_WITH_HTTP */

namespace Mmp
{
namespace RPC
{

RPCFactory::RPCFactory()
{
    RegisterBuiltins();
}

RPCFactory::~RPCFactory()
{

}

void RPCFactory::registerClass(const std::string& className, AbstractRPCInstantiator* instantiator)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _rpcFactory.registerClass(className, instantiator);
}

AbstractRPC::ptr RPCFactory::createRPC(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _rpcFactory.CreateInstance(className);
}

RPCFactory& RPCFactory::DefaultFactory()
{
    static RPCFactory gRPCFactory;
    return gRPCFactory;
}

void RPCFactory::RegisterBuiltins()
{
    std::lock_guard<std::mutex> lock(_mutex);
#ifdef RPC_WITH_HTTP
    _rpcFactory.registerClass("HTTPRPC", new Instantiator<HTTPRPC, AbstractRPC>);
#endif /* RPC_WITH_HTTP */
}

} // namespace RPC
} // namespace Mmp