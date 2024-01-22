#include "AbstractAllocateMethod.h"

namespace Mmp
{

bool AbstractAllocateMethod::Set(const std::string& /* key */, const Any& /* value */)
{
    return false;
}


Any AbstractAllocateMethod::Get(const std::string& /* key */)
{

    return Any();
}

} // namespace Mmp