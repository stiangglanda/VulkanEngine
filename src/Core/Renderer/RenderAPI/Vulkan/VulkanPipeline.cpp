#include "VulkanPipeline.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanDebug.h"

namespace Core
{

// Create the descriptor set layout
    void VulkanPipeline::createPipeline(VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass) 
    {

    //         VkShaderModule meshFragShader;
    // if (!vkutil::load_shader_module(RESOURCES_PATH "shaders/mesh.frag.spv", engine->_device, &meshFragShader))
    // {
    //     // fmt::println("Error when building the triangle fragment shader module");
    // }

    // VkShaderModule meshVertexShader;
    // if (!vkutil::load_shader_module(RESOURCES_PATH "shaders/mesh.vert.spv", engine->_device, &meshVertexShader))
    // {
    //     // fmt::println("Error when building the triangle vertex shader module");
    // }

    // VkPushConstantRange matrixRange{};
    // matrixRange.offset = 0;
    // matrixRange.size = sizeof(GPUDrawPushConstants);
    // matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // DescriptorLayoutBuilder layoutBuilder;
    // layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    // layoutBuilder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    // layoutBuilder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    // materialLayout = layoutBuilder.build(engine->_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    // VkDescriptorSetLayout layouts[] = {engine->_gpuSceneDataDescriptorLayout, materialLayout};

    // VkPipelineLayoutCreateInfo mesh_layout_info = vkinit::pipeline_layout_create_info();
    // mesh_layout_info.setLayoutCount = 2;
    // mesh_layout_info.pSetLayouts = layouts;
    // mesh_layout_info.pPushConstantRanges = &matrixRange;
    // mesh_layout_info.pushConstantRangeCount = 1;

    // VkPipelineLayout newLayout;
    // VK_CHECK(vkCreatePipelineLayout(engine->_device, &mesh_layout_info, nullptr, &newLayout));

    // opaquePipeline.layout = newLayout;
    // transparentPipeline.layout = newLayout;

    // build the stage-create-info for both vertex and fragment stages. This lets
    // the pipeline know the shader modules per stage

    auto vertShaderCode = readFile(RESOURCES_PATH "shaders/GLSL/vert.spv");
    auto fragShaderCode = readFile(RESOURCES_PATH "shaders/GLSL/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    // vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    // vertShaderStageInfo.module = vertShaderModule;
    // vertShaderStageInfo.pName = "main";

    // VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    // fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    // fragShaderStageInfo.module = fragShaderModule;
    // fragShaderStageInfo.pName = "main";

    // VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    PipelineBuilder pipelineBuilder;

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

    // VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    // inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // inputAssembly.primitiveRestartEnable = VK_FALSE;

    // VkPipelineViewportStateCreateInfo viewportState{};
    // viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // viewportState.viewportCount = 1;
    // viewportState.scissorCount = 1;

    // VkPipelineRasterizationStateCreateInfo rasterizer{};
    // rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // rasterizer.depthClampEnable = VK_FALSE;
    // rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // rasterizer.lineWidth = 1.0f;
    // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    // rasterizer.depthBiasEnable = VK_FALSE;

    // VkPipelineMultisampleStateCreateInfo multisampling{};
    // multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // VkPipelineDepthStencilStateCreateInfo depthStencil{};
    // depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    // depthStencil.depthTestEnable = VK_TRUE;
    // depthStencil.depthWriteEnable = VK_TRUE;
    // depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    // depthStencil.depthBoundsTestEnable = VK_FALSE;
    // depthStencil.stencilTestEnable = VK_FALSE;

    // VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    // colorBlendAttachment.colorWriteMask =
    //     VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // colorBlendAttachment.blendEnable = VK_FALSE;

    // VkPipelineColorBlendStateCreateInfo colorBlending{};
    // colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // colorBlending.logicOpEnable = VK_FALSE;
    // colorBlending.logicOp = VK_LOGIC_OP_COPY;
    // colorBlending.attachmentCount = 1;
    // colorBlending.pAttachments = &colorBlendAttachment;
    // colorBlending.blendConstants[0] = 0.0f;
    // colorBlending.blendConstants[1] = 0.0f;
    // colorBlending.blendConstants[2] = 0.0f;
    // colorBlending.blendConstants[3] = 0.0f;

    // std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    // VkPipelineDynamicStateCreateInfo dynamicState{};
    // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    // dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    // dynamicState.pDynamicStates = dynamicStates.data();

    // std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    // descriptorSetLayouts.push_back(descriptorSetLayout->get_handle());

    // pipelineLayout=std::make_unique<VulkanPipelineLayout>(device.getDevice(), descriptorSetLayouts);

    // VkGraphicsPipelineCreateInfo pipelineInfo{};
    // pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // pipelineInfo.stageCount = 2;
    // pipelineInfo.pStages = shaderStages;
    // pipelineInfo.pVertexInputState = &vertexInputInfo;
    // pipelineInfo.pInputAssemblyState = &inputAssembly;
    // pipelineInfo.pViewportState = &viewportState;
    // pipelineInfo.pRasterizationState = &rasterizer;
    // pipelineInfo.pMultisampleState = &multisampling;
    // pipelineInfo.pDepthStencilState = &depthStencil;
    // pipelineInfo.pColorBlendState = &colorBlending;
    // pipelineInfo.pDynamicState = &dynamicState;
    // pipelineInfo.layout = pipelineLayout->get_handle();
    // pipelineInfo.renderPass = renderPass;
    // pipelineInfo.subpass = 0;
    // pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    // if (vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
    //     VK_SUCCESS)
    // {
    //     throw std::runtime_error("failed to create graphics pipeline!");
    // }

    // vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
    // vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
    }

    // Helper function to destroy the descriptor set layout
    void VulkanPipeline::cleanup() {
        if (pipeline != VK_NULL_HANDLE) 
        {
            vkDestroyPipeline(device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }
        pipelineLayout.reset();
    }

    VkShaderModule VulkanPipeline::createShaderModule(const std::vector<char> &code)
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

    std::vector<char> VulkanPipeline::readFile(const std::string &filename)
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
