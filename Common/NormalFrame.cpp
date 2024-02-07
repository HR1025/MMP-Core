#include "NormalFrame.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalFrame::NormalFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod)
{
    this->_sharedData = std::make_shared<NormalSharedData>(size, allocateMethod);
}

} // namespace Mmp