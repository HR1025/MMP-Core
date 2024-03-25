#include "AbstractDeviceAllocateMethod.h"

namespace Mmp
{

const std::string AbstractDeviceAllocateMethod::kSync = "sync";
const std::string AbstractDeviceAllocateMethod::kMap = "map";
const std::string AbstractDeviceAllocateMethod::kUnmap = "unmap";

bool AbstractDeviceAllocateMethod::Set(const std::string& key, const Any& /* value */)
{
    if (key == AbstractDeviceAllocateMethod::kSync)
    {
        Sync();
        return true;
    }
    else if (key == AbstractDeviceAllocateMethod::kMap)
    {
        Map();
        return true;
    }
    else if (key == AbstractDeviceAllocateMethod::kUnmap)
    {
        UnMap();
        return true;
    }
    else
    {
        return false;
    }
}

int AbstractDeviceAllocateMethod::GetFd()
{
    return -1;
}

} // namespace Mmp