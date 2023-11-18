#include "MediaType.h"

#include <cassert>

namespace Mmp
{

const std::string MediaTypeToStr(MeidaType type)
{
    switch (type)
    {
        case MeidaType::AUDIO: return "AUDIO";
        case MeidaType::VIDEO: return "VIDEO";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, MeidaType type)
{
    os << MediaTypeToStr(type);
    return os;
}

} // namespace Mmp