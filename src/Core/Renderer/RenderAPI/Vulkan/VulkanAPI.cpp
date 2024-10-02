#include "VulkanAPI.h"
#include "../../../vepch.h"

#include <GLFW/glfw3.h>
#include <memory>

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

    renderPass=std::make_unique<VulkanRenderPass>(device, swapChain);
    descriptorSetLayout=std::make_unique<VulkanDescriptorSetLayout>(device.getDevice());
    graphicsPipeline=std::make_unique<VulkanPipeline>(device.getDevice(),
                                                      descriptorSetLayout->get_handle(),
                                                      renderPass->get_handle());
    command = std::make_shared<VulkanCommandBuffer>(device);
    command->createCommandPool(surface.getSurface(), MAX_FRAMES_IN_FLIGHT);

    swapChain.createDepthResourcesAndFramebuffers(device, renderPass->get_handle());

    texture = ImageBuilder(0, 0) // with the option with_texture this will be set later
                  .with_format(VK_FORMAT_R8G8B8A8_SRGB)
                  .with_image_type(VK_IMAGE_TYPE_2D)
                  .with_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                  .with_tiling(VK_IMAGE_TILING_OPTIMAL)
                  .with_vma_usage(VMA_MEMORY_USAGE_AUTO)
                  .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                  .with_texture(TEXTURE_PATH, command)
                  .build_shared(device);

    texture->createTextureImageView();
    texture->createTextureSampler();

    model = std::make_unique<VulkanModel>(MODEL_PATH, device, command);

    createUniformBuffers();//TODO should probobly be in VulkanModel
    descriptorSet = std::make_unique<VulkanDescriptorSet>(device.getDevice(),//TODO should probobly be in VulkanModel
                                                        MAX_FRAMES_IN_FLIGHT,
                                                        descriptorSetLayout->get_handle(), 
                                                        uniformBuffers,texture);

    command->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);

    createSyncObjects();
    // glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // Cam.velocity = glm::vec3(0.f);

    // Cam.position = glm::vec3(2.0f, 2.0f, 2.0f);

    // Cam.pitch = 0;
    // Cam.yaw = 0;
    return true;
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
    ubo.model = model->getModelMatrix();//TODO shoul be seperate from camera cause per object
    // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.getExtent().width / (float)swapChain.getExtent().height,
                                0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
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
        swapChain.recreateSwapChain(device, surface.getSurface(), renderPass->get_handle());
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
        swapChain.recreateSwapChain(device, surface.getSurface(), renderPass->get_handle());
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

    graphicsPipeline.reset();
    
    renderPass.reset();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        uniformBuffers[i].reset(); // This is to avoid a segmentation fault since the memory
                                   // already gets freed by VulkanMemoryManager::shutdown and
                                   // then the destructore tryes to do the same therefore we call the destructure before
    }

    descriptorSet.reset();
    texture.reset();

    descriptorSetLayout.reset();

    model.reset();
    // This is to avoid a segmentation fault since the memory
    // already gets freed by VulkanMemoryManager::shutdown and
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
    renderPassInfo.renderPass = renderPass->get_handle();
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

    VkBuffer vertexBuffers[] = {model->get_VertexBuffer_handle()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command->getCommandBuffer(currentFrame), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command->getCommandBuffer(currentFrame), model->get_IndexBuffer_handle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(command->getCommandBuffer(currentFrame), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get_PipelineLayout(), 0,
                            1, descriptorSet->get_handle_ptr_at_index(currentFrame), 0, nullptr);
    vkCmdDrawIndexed(command->getCommandBuffer(currentFrame), static_cast<uint32_t>(model->getIndicesSize()), 1, 0, 0, 0);

    vkCmdEndRenderPass(command->getCommandBuffer(currentFrame));

    command->end(currentFrame);
}

} // namespace Core
