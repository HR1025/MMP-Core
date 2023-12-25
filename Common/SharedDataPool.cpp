#include "SharedDataPool.h"

#include <cassert>
#include <functional>

#include "SharedData.h"
#include "AllocateMethodFactory.h"
#include "SharedDataDecorator.h"

namespace Mmp
{

class SharedDataAvailableListener : public SharedDataDecorator
{
public:
    using ptr = std::shared_ptr<SharedDataAvailableListener>;
    using OnAvailable = std::function<void (size_t slot)>;
public:
    SharedDataAvailableListener(AbstractSharedData::ptr sharedData, size_t slot, OnAvailable onAvailable);
    ~SharedDataAvailableListener();
private:
    size_t _slot;
    OnAvailable _onAvailable;
};

SharedDataAvailableListener::SharedDataAvailableListener(AbstractSharedData::ptr sharedData, size_t slot, OnAvailable onAvailable)
    : SharedDataDecorator(sharedData)
{
    _slot = slot;
    _onAvailable = onAvailable;
}

SharedDataAvailableListener::~SharedDataAvailableListener()
{
    _onAvailable(_slot);
}

} // namespace Mmp

namespace Mmp
{

SharedDataPool::SharedDataPool(size_t dataSize, size_t capacity, AbstractAllocateMethod::ptr allocateMethod)
{
    _allocateMethod = allocateMethod;
    _dataSize = dataSize;
    _curSlot = 0;
    _capacity = capacity;
    _usedSlot = 0;
    _availableSlot = 0;
    _allocatedSlot = 0;

    if (!_allocateMethod)
    {
        _allocateMethod = AllocateMethodFactory::DefaultFactory().CreateAllocateMethod("NormalAllocateMethod");
    }

    for (size_t i=0; i<_capacity; i++)
    {
        _idles.insert(_curSlot);
        _availableSlot++;
        _curSlot++;
    }

}

void SharedDataPool::AddCapacity(size_t n)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _capacity += n;
    for (size_t i=0; i<n; i++)
    {
        _idles.insert(_curSlot);
        _availableSlot++;
        _curSlot++;
    }
    _cond.notify_all();
}

void SharedDataPool::DelCapacity(size_t n)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (n > _capacity)
    {
        _capacity = 0;        
    }
    else
    {
        _capacity -= n;
    }
    // Hint : 尝试回收一部分内存块
    for (size_t i=0; i<n; i++)
    {
        if (_idles.empty())
        {
            break;
        }
        uint32_t slot = *_idles.begin();
        _idles.erase(slot);
        _availableSlot--;
        if (_buffers.count(slot))
        {
            _buffers.erase(slot);
            _allocatedSlot--;
        }
    }
}

size_t SharedDataPool::Capacity()
{
    return _capacity;
}

size_t SharedDataPool::Used()
{
    return _usedSlot;
}

size_t SharedDataPool::Available()
{
    return _availableSlot;
}

size_t SharedDataPool::Allocated()
{
    return _allocatedSlot;
}

AbstractSharedData::ptr SharedDataPool::Request(uint32_t waitTimeMs)
{
    uint32_t slot = 0;
    AbstractSharedData::ptr buffer = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mtx);
        if (_idles.empty())
        {
            if (waitTimeMs == 0)
            {
                _cond.wait(lock);
            }
            else
            {
                _cond.wait_for(lock, std::chrono::milliseconds(waitTimeMs));
            }
        }
        if (_idles.empty())
        {
            return nullptr;
        }
        slot =  *_idles.begin();
        _idles.erase(slot);
        _availableSlot--;
        _occupys.insert(slot);
        _usedSlot++;
    }
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_buffers.count(slot) == 0)
        {
            _buffers[slot] = std::make_shared<SharedData>(_dataSize, _allocateMethod);
            _allocatedSlot++;
        }
        buffer = _buffers[slot];
    }
    std::weak_ptr<SharedDataPool> weak = shared_from_this();
    return std::make_shared<SharedDataAvailableListener>(buffer, slot, [weak, this](size_t slot) -> void
    {
        SharedDataPool::ptr strong = weak.lock();
        if (!strong)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(this->_mtx);
        if (this->_occupys.count(slot))
        {
            this->_occupys.erase(slot);
            this->_usedSlot--;
        }
        if (this->_allocatedSlot > this->_capacity) // Hint : 回收动态减少容量多余的部分
        {
            this->_buffers.erase(slot);
            this->_allocatedSlot--;
        }
        else
        {
            this->_idles.insert(slot);
            this->_availableSlot++;
            this->_cond.notify_one();
        }
    });
}

}; // namespace Mmp