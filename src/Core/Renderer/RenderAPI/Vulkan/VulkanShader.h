#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core
{
    namespace VulkanShader
    {
        VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code);//TODO should be in VulkanShader
        std::vector<char> readFile(const std::string &filename);//TODO should be in VulkanShader
    }
} // namespace Core
