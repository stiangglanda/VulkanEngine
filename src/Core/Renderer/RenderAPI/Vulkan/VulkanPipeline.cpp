#include "VulkanPipeline.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanDebug.h"

namespace Core
{
    void VulkanPipeline::createPipeline(VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass) 
    {
        auto vertShaderCode = readFile(RESOURCES_PATH "shaders/GLSL/vert.spv");
        auto fragShaderCode = readFile(RESOURCES_PATH "shaders/GLSL/frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VulkanPipelineBuilder pipelineBuilder;

        pipelineBuilder.set_shaders(vertShaderModule, fragShaderModule);

        pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);

        pipelineBuilder.set_cull_mode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);

        pipelineBuilder.set_multisampling_none();

        pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_LESS);

        pipelineBuilder.disable_blending();

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.push_back(descriptorSetLayout);

        pipelineLayout=std::make_unique<VulkanPipelineLayout>(device, descriptorSetLayouts);

        // use the triangle layout we created
        pipelineBuilder._pipelineLayout = pipelineLayout->get_handle();

        pipelineBuilder._renderPass = renderPass;

        // render format
        // pipelineBuilder.set_color_attachment_format(engine->_drawImage.imageFormat);
        // pipelineBuilder.set_depth_format(engine->_depthImage.imageFormat);

        // finally build the pipeline
        pipeline = pipelineBuilder.build_pipeline(device);

        // create the transparent variant
        // pipelineBuilder.enable_blending_additive();

        // pipelineBuilder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

        // transparentPipeline.pipeline = pipelineBuilder.build_pipeline(engine->_device);

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);

        VE_CORE_INFO("Created Pipeline");
    }

    void VulkanPipeline::cleanup() {
        if (pipeline != VK_NULL_HANDLE) 
        {
            vkDestroyPipeline(device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
            VE_CORE_INFO("Destroyed Pipeline");
        }
        pipelineLayout.reset();
    }

    VkShaderModule VulkanPipeline::createShaderModule(const std::vector<char> &code)//TODO whould be in VulkanShader
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

    std::vector<char> VulkanPipeline::readFile(const std::string &filename)//TODO whould be in VulkanShader
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

} // namespace Core
