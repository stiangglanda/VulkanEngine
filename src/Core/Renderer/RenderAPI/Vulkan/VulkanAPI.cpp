#include "VulkanAPI.h"
#include "../../../vepch.h"

#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "../../../Application.h"
#include "VulkanBuffer.h"

#include "VulkanMemoryManager.h"

const std::string MODEL_PATH = RESOURCES_PATH "viking_room.obj";
const std::string TEXTURE_PATH = RESOURCES_PATH "viking_room.png";

namespace Core
{

bool VulkanAPI::Init()
{
    instance.Init(enableValidationLayers, validationLayers);
    vkDebug.Init(instance.getInstance(), enableValidationLayers);
    surface.Init(instance.getInstance());
    device.Init(instance.getInstance(), surface.getSurface(), enableValidationLayers, validationLayers);
    VulkanMemoryManager::init(device, instance.getInstance());
    swapChain.Init(device, surface.getSurface());

    createRenderPass();
    descriptorSetLayout=std::make_shared<VulkanDescriptorSetLayout>(device.getDevice());
    graphicsPipeline=std::make_unique<VulkanPipeline>(device.getDevice(),descriptorSetLayout->get_handle(),renderPass);
    // createGraphicsPipeline();
    command = std::make_shared<VulkanCommandBuffer>(device);
    command->createCommandPool(surface.getSurface(), MAX_FRAMES_IN_FLIGHT);

    swapChain.createDepthResourcesAndFramebuffers(device, renderPass);

    texture = ImageBuilder(0, 0) // with the option with_texture this will be set later
                  .with_format(VK_FORMAT_R8G8B8A8_SRGB)
                  .with_image_type(VK_IMAGE_TYPE_2D)
                  .with_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                  .with_tiling(VK_IMAGE_TILING_OPTIMAL)
                  .with_vma_usage(VMA_MEMORY_USAGE_AUTO)
                  .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                  .with_texture(TEXTURE_PATH, command)
                  .build_unique(device);

    texture->createTextureImageView();
    texture->createTextureSampler();

    loadModel();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();

    command->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);

    createSyncObjects();
    // glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // Cam.velocity = glm::vec3(0.f);

    // Cam.position = glm::vec3(2.0f, 2.0f, 2.0f);

    // Cam.pitch = 0;
    // Cam.yaw = 0;
    return true;
}

void VulkanAPI::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChain.getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = device.findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device.getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

// void VulkanAPI::createGraphicsPipeline()
// {
//     auto vertShaderCode = readFile(RESOURCES_PATH "shaders/GLSL/vert.spv");
//     auto fragShaderCode = readFile(RESOURCES_PATH "shaders/GLSL/frag.spv");

//     VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
//     VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

//     VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//     vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//     vertShaderStageInfo.module = vertShaderModule;
//     vertShaderStageInfo.pName = "main";

//     VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//     fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//     fragShaderStageInfo.module = fragShaderModule;
//     fragShaderStageInfo.pName = "main";

//     VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

//     VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//     vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

//     auto bindingDescription = Vertex::getBindingDescription();
//     auto attributeDescriptions = Vertex::getAttributeDescriptions();

//     vertexInputInfo.vertexBindingDescriptionCount = 1;
//     vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//     vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//     vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

//     VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//     inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//     inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//     inputAssembly.primitiveRestartEnable = VK_FALSE;

//     VkPipelineViewportStateCreateInfo viewportState{};
//     viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//     viewportState.viewportCount = 1;
//     viewportState.scissorCount = 1;

//     VkPipelineRasterizationStateCreateInfo rasterizer{};
//     rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//     rasterizer.depthClampEnable = VK_FALSE;
//     rasterizer.rasterizerDiscardEnable = VK_FALSE;
//     rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//     rasterizer.lineWidth = 1.0f;
//     rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//     rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//     rasterizer.depthBiasEnable = VK_FALSE;

//     VkPipelineMultisampleStateCreateInfo multisampling{};
//     multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//     multisampling.sampleShadingEnable = VK_FALSE;
//     multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

//     VkPipelineDepthStencilStateCreateInfo depthStencil{};
//     depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//     depthStencil.depthTestEnable = VK_TRUE;
//     depthStencil.depthWriteEnable = VK_TRUE;
//     depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
//     depthStencil.depthBoundsTestEnable = VK_FALSE;
//     depthStencil.stencilTestEnable = VK_FALSE;

//     VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//     colorBlendAttachment.colorWriteMask =
//         VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//     colorBlendAttachment.blendEnable = VK_FALSE;

//     VkPipelineColorBlendStateCreateInfo colorBlending{};
//     colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//     colorBlending.logicOpEnable = VK_FALSE;
//     colorBlending.logicOp = VK_LOGIC_OP_COPY;
//     colorBlending.attachmentCount = 1;
//     colorBlending.pAttachments = &colorBlendAttachment;
//     colorBlending.blendConstants[0] = 0.0f;
//     colorBlending.blendConstants[1] = 0.0f;
//     colorBlending.blendConstants[2] = 0.0f;
//     colorBlending.blendConstants[3] = 0.0f;

//     std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
//     VkPipelineDynamicStateCreateInfo dynamicState{};
//     dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//     dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//     dynamicState.pDynamicStates = dynamicStates.data();

//     std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
//     descriptorSetLayouts.push_back(descriptorSetLayout->get_handle());

//     pipelineLayout=std::make_unique<VulkanPipelineLayout>(device.getDevice(), descriptorSetLayouts);

//     VkGraphicsPipelineCreateInfo pipelineInfo{};
//     pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//     pipelineInfo.stageCount = 2;
//     pipelineInfo.pStages = shaderStages;
//     pipelineInfo.pVertexInputState = &vertexInputInfo;
//     pipelineInfo.pInputAssemblyState = &inputAssembly;
//     pipelineInfo.pViewportState = &viewportState;
//     pipelineInfo.pRasterizationState = &rasterizer;
//     pipelineInfo.pMultisampleState = &multisampling;
//     pipelineInfo.pDepthStencilState = &depthStencil;
//     pipelineInfo.pColorBlendState = &colorBlending;
//     pipelineInfo.pDynamicState = &dynamicState;
//     pipelineInfo.layout = pipelineLayout->get_handle();
//     pipelineInfo.renderPass = renderPass;
//     pipelineInfo.subpass = 0;
//     pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

//     if (vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
//         VK_SUCCESS)
//     {
//         throw std::runtime_error("failed to create graphics pipeline!");
//     }

//     vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
//     vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
// }

void VulkanAPI::loadModel()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto &shape : shapes)
    {
        for (const auto &index : shape.mesh.indices)
        {
            Vertex vertex{};
            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                               1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void VulkanAPI::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, bufferSize, vertices.data());

    vertexBuffer = (BufferBuilder(bufferSize)
                        .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                        .with_vma_usage(VMA_MEMORY_USAGE_AUTO) // VMA_MEMORY_USAGE_GPU_ONLY
                        .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                        .build_unique(device));
    VulkanBuffer::copyBuffer(command, stagingBuffer.get_handle(), vertexBuffer->get_handle(), bufferSize);
}

void VulkanAPI::createIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, bufferSize, indices.data());

    indexBuffer = (BufferBuilder(bufferSize)
                       .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                       .with_vma_usage(VMA_MEMORY_USAGE_AUTO) // VMA_MEMORY_USAGE_GPU_ONLY
                       .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                       .build_unique(device));
    VulkanBuffer::copyBuffer(command, stagingBuffer.get_handle(), indexBuffer->get_handle(), bufferSize);
}

void VulkanAPI::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        uniformBuffers[i] = (BufferBuilder(bufferSize)
                                 .with_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                                 .with_vma_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
                                 .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                                 .build_unique(device));
        uniformBuffersMapped[i] = uniformBuffers[i]->map();
    }
}

void VulkanAPI::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanAPI::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout->get_handle());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();
    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i]->get_handle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->getImageView();
        imageInfo.sampler = texture->getSampler();
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device.getDevice(), static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void VulkanAPI::createSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanAPI::OnEvent(Core::Event &e, float delta)
{
    // if (e.IsInCategory(Core::EventCategory::EventCategoryApplication))
    //{
    //     if (e.GetEventType() == Core::EventType::WindowResize)
    //     {
    //         resize_requested = true;
    //         e.Handled = true;
    //     }
    // }
}

void VulkanAPI::updateUniformBuffer(uint32_t currentImage) // TODO use actual camera
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // Cam.update();
    UniformBufferObject ubo{};

    ubo.view = Cam.getViewMatrix();

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::mat4(1.0f);
    // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.getExtent().width / (float)swapChain.getExtent().height,
                                0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

VkShaderModule VulkanAPI::createShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> VulkanAPI::readFile(const std::string &filename)
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

void VulkanAPI::Update(float delta)
{
    Cam.processEvent(delta);
}

void VulkanAPI::Draw()
{
    vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device.getDevice(), swapChain.getSwapChain(), UINT64_MAX,
                                            imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        swapChain.recreateSwapChain(device, surface.getSurface(), renderPass);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(currentFrame);

    vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

    command->ResetCommandBuffer(currentFrame);
    recordCommandBuffer(currentFrame, imageIndex);

    command->submit(currentFrame, inFlightFences[currentFrame], imageAvailableSemaphores[currentFrame],
                    renderFinishedSemaphores[currentFrame]);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

    VkSwapchainKHR swapChains[] = {swapChain.getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        swapChain.recreateSwapChain(device, surface.getSurface(), renderPass);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

bool VulkanAPI::Shutdown()
{
    swapChain.Shutdown(device.getDevice());

    // vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
    graphicsPipeline.reset();
    // pipelineLayout.reset();

    vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        uniformBuffers[i].reset(); // This is to avoid a segmentation fault since the memory
                                   // already gets freed by VulkanMemoryManager::shutdown and
                                   // then the destructore tryes to do the same therefore we call the destructure before
    }

    vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
    texture.reset();

    descriptorSetLayout.reset();
    indexBuffer.reset();  // This is to avoid a segmentation fault since the memory
    vertexBuffer.reset(); // already gets freed by VulkanMemoryManager::shutdown and
                          // then the destructore tryes to do the same therefore we call the destructure before

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.getDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.getDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.getDevice(), inFlightFences[i], nullptr);
    }
    command->Shutdown();

    VulkanMemoryManager::shutdown();

    device.Shutdown();

    vkDebug.Shutdown(instance.getInstance());
    surface.Shutdown(instance.getInstance());
    instance.Shutdown();

    return true;
}

void VulkanAPI::recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex)
{
    command->begin(currentFrame);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChain.getFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain.getExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(command->getCommandBuffer(currentFrame), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command->getCommandBuffer(currentFrame), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get_handle());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain.getExtent().width;
    viewport.height = (float)swapChain.getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command->getCommandBuffer(currentFrame), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain.getExtent();
    vkCmdSetScissor(command->getCommandBuffer(currentFrame), 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {vertexBuffer->get_handle()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command->getCommandBuffer(currentFrame), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command->getCommandBuffer(currentFrame), indexBuffer->get_handle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(command->getCommandBuffer(currentFrame), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get_PipelineLayout(), 0,
                            1, &descriptorSets[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(command->getCommandBuffer(currentFrame), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(command->getCommandBuffer(currentFrame));

    command->end(currentFrame);
}

} // namespace Core
