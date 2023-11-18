#include "AbstractRPCProcess.h"

namespace Mmp
{
namespace RPC
{

FuncRPCProcess::FuncRPCProcess(RPCProcess process)
{
    _process = process;
}

bool FuncRPCProcess::Process(const Any& request, Any& response)
{
    return _process(request, response);
}

} // namespace RPC
} // namespace Mmp