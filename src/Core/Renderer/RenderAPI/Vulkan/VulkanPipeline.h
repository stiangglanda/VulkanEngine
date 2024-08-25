#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core
{

class VulkanPipeline
{
  public:
    VulkanPipeline()
    {
    }
    ~VulkanPipeline()
    {
    }

    void Init();

    void Shutdown();

  private:
};
} // namespace Core
