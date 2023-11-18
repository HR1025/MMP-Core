#include "VulkanWindowsCommon.h"

#include <cassert>

namespace Mmp
{

std::string VkPresentModeKHRToStr(VkPresentModeKHR mode)
{
    switch (mode)
    {
        case VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR: return "IMMEDIATE";
        case VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR: return "MALIBOX";
        case VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR: return "FIFO";
        case VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR: return "FIFO_RELAXED";
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR: return "SHARED_DEMAND_REFRESH_KHR";
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: return "SHARED_CONTINUOUS_REFRESH_KHR";
        default: assert(false); return "UNKNOWN";
    }
}

} // namespace Mmp