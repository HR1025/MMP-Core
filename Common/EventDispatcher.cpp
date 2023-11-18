#include "EventDispatcher.h"

namespace Mmp
{

void EventDispatcher::DelListener(void* tag, bool& isEmpty)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _listeners.erase(tag);
    isEmpty = _listeners.empty();
}

} // namespace Mmp