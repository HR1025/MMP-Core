#include "AbstractRPCDispatcher.h"

namespace Mmp
{
namespace RPC
{

SimpleRPCDispatcher::SimpleRPCDispatcher(const std::string strategy)
{
    _strategy = strategy;
}

bool SimpleRPCDispatcher::Match(const std::string& strategy)
{
    return _strategy == strategy;
}

} // namespace RPC
} // namespace Mmp