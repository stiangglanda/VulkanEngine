#include "VulkanShader.h"
#include "VulkanDebug.h"

namespace Core
{
    namespace VulkanShader
    {
        VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code)//TODO whould be in VulkanShader
        {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
        }

        // VkShaderModule loadShaderModule(VkDevice device, const std::string &filename)//TODO whould be in VulkanShader
        // {
        //     return createShaderModule(device, readFile(filename));
        // }

        bool loadShaderModule(VkDevice device, const std::string &filename, VkShaderModule* outShaderModule)
        {
            std::vector<char> shaderCode = readFile(filename);

            if (shaderCode.empty())
            {
                std::cerr << "Failed to read shader file: " << filename << std::endl;
                return false;
            }

            VkShaderModule shaderModule = createShaderModule(device, shaderCode);

            if (shaderModule == VK_NULL_HANDLE) // Assuming this is returned in case of failure
            {
                std::cerr << "Failed to create shader module from file: " << filename << std::endl;
                return false;
            }

            *outShaderModule = shaderModule;
            return true;
        }

        std::vector<char> readFile(const std::string &filename)//TODO whould be in VulkanShader
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                throw std::runtime_error("failed to open file!");
            }

            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }

        VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage,
                                                                          VkShaderModule shaderModule,
                                                                          const char *entry)
        {
            VkPipelineShaderStageCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext = nullptr;

            // shader stage
            info.stage = stage;
            // module containing the code for this shader stage
            info.module = shaderModule;
            // the entry point of the shader
            info.pName = entry;
            return info;
        }
    }
} // namespace Core
