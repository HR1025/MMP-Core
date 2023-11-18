#include "AbstractRPCResponse.h"

namespace Mmp
{
namespace RPC
{

FuncRPCResponse::FuncRPCResponse(RPCSerialize serialize)
{
    _serialize = serialize;
}

bool FuncRPCResponse::Serialize(const Any& form, SharedData::ptr& to)
{
    return _serialize(form, to);
}

} // namespace RPC
} // namespace Mmp
