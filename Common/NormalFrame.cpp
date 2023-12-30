#include "NormalFrame.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalFrame::NormalFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod)
{
    this->_sharedData = std::make_shared<NormalFrame>(size, allocateMethod);
}

} // namespace Mmp