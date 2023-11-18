#include "AbstractRPCRequest.h"

namespace Mmp
{
namespace RPC
{

FuncRPCRequest::FuncRPCRequest(RPCDeserialize deserialize)
{
    _deserialize = deserialize;
}

bool FuncRPCRequest::Deserialize(SharedData::ptr from, Any& to)
{
    return _deserialize(from, to);
}

} // namespace RPC
} // namespace Mmp