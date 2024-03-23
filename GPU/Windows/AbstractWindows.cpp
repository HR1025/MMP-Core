#include "AbstractWindows.h"


namespace Mmp
{

void* AbstractWindows::GetProcAddress(const std::string& funcName)
{
    return nullptr;
}

void AbstractWindows::Set(const std::string& /* key */, Any /* value */)
{

}

Any AbstractWindows::Get(const std::string& /* key */)
{
    return Any();
}

} // namespace Mmp