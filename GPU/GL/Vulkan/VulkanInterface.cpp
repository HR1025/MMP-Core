#include "VulkanInterface.h"

#include "VulkanCommon.h"

namespace Mmp
{

static VulkanInterface::ptr gImpl = nullptr;

void VulkanInterface::Register(VulkanInterface::ptr impl)
{
    gImpl = impl;
}

void VulkanInterface::UnRegister()
{
    gImpl.reset();
}

VulkanInterface::ptr VulkanInterface::Instance()
{
    return gImpl;
}

} // namespace Mmp