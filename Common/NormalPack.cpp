#include "NormalPack.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalPack::NormalPack(size_t size, AbstractAllocateMethod::ptr allocateMethod)
{
    this->_sharedData = std::make_shared<NormalSharedData>(size, allocateMethod);
}

} // namespace Mmp