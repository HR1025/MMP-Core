#include "EventMessage.h"

namespace Mmp
{

EventMessage::EventMessage(const EventMessage&& that)
{
    _type = that._type;
    _data = std::move(that._data);
}

EventMessage& EventMessage::operator=(const EventMessage&& that)
{
    _type = that._type;
    _data = std::move(that._data);
    return *this;
}

void EventMessage::Reset()
{
    _type = nullptr;
    _data = nullptr;
}

} // namespace Mmp