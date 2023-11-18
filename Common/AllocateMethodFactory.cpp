#include "AllocateMethodFactory.h"

#include <Poco/SingletonHolder.h>

#include "NormalAllocateMethod.h"
#include "SegmentAllocateMethod.h"

namespace Mmp
{

namespace
{
    static Poco::SingletonHolder<AllocateMethodFactory> sh;
}

AllocateMethodFactory::AllocateMethodFactory()
{
    RegisterBuiltins();
}

AllocateMethodFactory::~AllocateMethodFactory()
{

}

void AllocateMethodFactory::RegisterAllocateMethodClass(const std::string& className, AllocateMethodInstantiator* instantiator)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _allocateMethodFactory.registerClass(className, instantiator);
}

AbstractAllocateMethod::ptr AllocateMethodFactory::CreateAllocateMethod(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _allocateMethodFactory.CreateInstance(className);
}

void AllocateMethodFactory::RegisterBuiltins()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _allocateMethodFactory.registerClass("NormalAllocateMethod", new Instantiator<NormalAllocateMethod, AbstractAllocateMethod>);
    _allocateMethodFactory.registerClass("SegmentAllocateMethod", new Instantiator<SegmentAllocateMethod, AbstractAllocateMethod>);
}

AllocateMethodFactory& AllocateMethodFactory::DefaultFactory()
{
    return *sh.get();
}

}