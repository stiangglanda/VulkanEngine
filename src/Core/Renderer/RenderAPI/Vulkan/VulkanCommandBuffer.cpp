#include "VulkanCommandBuffer.h"

namespace Core 
{

void VulkanCommandBuffer::Shutdown(VkInstance instance)
{

    VE_CORE_INFO("Shutdown Vulkan Debug");

}

void VulkanCommandBuffer::Init(VkInstance instance, const bool enableValidationLayers)
{
    VE_CORE_INFO("Init VulkanCommandBuffer");
}
} // namespace Core