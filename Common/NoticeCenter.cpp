#include "NoticeCenter.h"

namespace Mmp
{

NoticeCenter::ptr NoticeCenter::Instance()
{
    static NoticeCenter::ptr gInstance = std::make_shared<NoticeCenter>();
    return gInstance;
}

EventDispatcher::ptr NoticeCenter::GetDispatcher(const std::string& event, bool create)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_dispatchers.count(event))
    {
        if (create)
        {
            _dispatchers[event] = std::make_shared<EventDispatcher>();
        }
        else
        {
            return nullptr;
        }
    }
    return _dispatchers[event];
}

void NoticeCenter::DelListener(const std::string& event, void* tag)
{
    EventDispatcher::ptr dispatcher = GetDispatcher(event);
    if (!dispatcher)
    {
        return;
    }
    bool empty = false;
    dispatcher->DelListener(tag, empty);
    if (empty)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _dispatchers.erase(event);
    }
}

} // namespace Mmp