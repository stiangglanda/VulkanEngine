#pragma once
#include "../../../vepch.h"
#include <volk.h>

namespace Core
{
    namespace VulkanShader
    {
        VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code);
        bool loadShaderModule(VkDevice device, const std::string &filename, VkShaderModule* outShaderModule);
        std::vector<char> readFile(const std::string &filename);//TODO the vector should be empty when there is an error instead of throwing a runtime error
        VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage,
                                                                          VkShaderModule shaderModule,
                                                                          const char *entry = "main");
    }
} // namespace Core
