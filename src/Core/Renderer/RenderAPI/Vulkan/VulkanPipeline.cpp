#include "VulkanPipeline.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanDebug.h"
#include "VulkanShader.h"

namespace Core
{
    void VulkanPipeline::createPipeline(VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass) 
    {

        VkShaderModule vertShaderModule;
        if(!VulkanShader::loadShaderModule(device, RESOURCES_PATH "shaders/GLSL/vert.spv", &vertShaderModule))
        {
            VE_CORE_ERROR("Failed to load Shader");
        }
        VkShaderModule fragShaderModule;
        if(!VulkanShader::loadShaderModule(device, RESOURCES_PATH "shaders/GLSL/frag.spv", &fragShaderModule))
        {
            VE_CORE_ERROR("Failed to load Shader");
        }

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

        vkDestroyShaderModule(device, fragShaderModule, nullptr);//TODO maybe create a VulkanShader class anyway so this can be called by the destructor
        vkDestroyShaderModule(device, vertShaderModule, nullptr);//TODO maybe create a VulkanShader class anyway so this can be called by the destructor

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

} // namespace Core
