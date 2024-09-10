#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core
{
    namespace VulkanShader
    {
        VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code);//TODO should be in VulkanShader
        VkShaderModule loadShaderModule(VkDevice device, const std::string &filename);//TODO should be in VulkanShader
        std::vector<char> readFile(const std::string &filename);//TODO should be in VulkanShader
        VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage,
                                                                          VkShaderModule shaderModule,
                                                                          const char *entry = "main");
    }
} // namespace Core
