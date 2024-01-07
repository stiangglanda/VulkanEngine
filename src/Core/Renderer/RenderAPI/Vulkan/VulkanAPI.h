#pragma once
#include "../../../vepch.h"
#include "../../RenderAPI.h"
#include <vulkan/vulkan.h>

namespace Core
{

class VulkanAPI : public RenderAPI
{
  public:
    VulkanAPI();
    virtual ~VulkanAPI() = default;
};
} // namespace Core
