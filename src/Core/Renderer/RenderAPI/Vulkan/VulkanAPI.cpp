#include "VulkanAPI.h"
#include "../../../vepch.h"

#include <GLFW/glfw3.h>
#include <memory>

#include "../../../Application.h"
#include "VulkanBuffer.h"

#include "VulkanMemoryManager.h"
#include "VulkanModel.h"
#include "VulkanSync.h"
#include "../../../SceneGraph/MeshNode.h"

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

    model = std::make_unique<VulkanModel>(MODEL_PATH, TEXTURE_PATH, device, command, MAX_FRAMES_IN_FLIGHT);

    descriptorSet = std::make_unique<VulkanDescriptorSet>(device.getDevice(),//TODO should probobly be in VulkanModel
                                                        MAX_FRAMES_IN_FLIGHT,
                                                        descriptorSetLayout->get_handle(), 
                                                        model->get_uniformBuffers(),model->get_texture());

    command->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);

    sync=std::make_unique<VulkanSync>(device.getDevice(),MAX_FRAMES_IN_FLIGHT);

    return true;
}

std::shared_ptr<Model> VulkanAPI::LoadModel(const std::string model_path, const std::string texture_path)
{
    return std::make_shared<VulkanModel>(model_path, texture_path, device, command, MAX_FRAMES_IN_FLIGHT);
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

void VulkanAPI::Update(float delta)
{
    Cam.processEvent(delta);
}

void VulkanAPI::Draw()
{
    vkWaitForFences(device.getDevice(), 1, 
           sync->get_inFlightFences_handle_by_index(currentFrame), 
           VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device.getDevice(), swapChain.getSwapChain(), UINT64_MAX,
                                 sync->get_imageAvailableSemaphores_handle_by_index(currentFrame), 
                                     VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        swapChain.recreateSwapChain(device, surface.getSurface(), renderPass->get_handle());
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    
    model->updateUniformBuffer(currentFrame, Cam, swapChain.getExtent().width / (float)swapChain.getExtent().height);

    vkResetFences(device.getDevice(), 1, sync->get_inFlightFences_handle_by_index(currentFrame));

    command->ResetCommandBuffer(currentFrame);
    recordCommandBuffer(currentFrame, imageIndex);

    command->submit(currentFrame, *sync->get_inFlightFences_handle_by_index(currentFrame), 
     sync->get_imageAvailableSemaphores_handle_by_index(currentFrame),
  *sync->get_renderFinishedSemaphores_handle_by_index(currentFrame));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = sync->get_renderFinishedSemaphores_handle_by_index(currentFrame);

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

    descriptorSet.reset();

    descriptorSetLayout.reset();

    model.reset();
    
    // This is to avoid a segmentation fault since the memory
    // already gets freed by VulkanMemoryManager::shutdown and
    // then the destructore tryes to do the same therefore we call the destructure before

    sync.reset();
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

    vkCmdBeginRenderPass(command->getCommandBuffer(currentFrame), 
       &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command->getCommandBuffer(currentFrame), 
    VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get_handle());

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

    if(Application::Get().GetSceneGraph().getRoot()->getChildren()[0]->getName()=="Mesh")//TODO turn this into a loop and figure out how i can detect if something is a MeshNode
    {
        std::shared_ptr<VulkanModel> sgModel=std::dynamic_pointer_cast<VulkanModel>(Application::Get().GetSceneGraph().getRoot()->getChildren()[0]->getModel());
        if(!sgModel) 
        {
            VE_CORE_ERROR("Failed to cast Model to VulkanModel");
            return;
        }
        sgModel->updateUniformBuffer(currentFrame, Cam, swapChain.getExtent().width / (float)swapChain.getExtent().height);

        VkBuffer vertexBuffers[] = {sgModel->get_VertexBuffer_handle()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command->getCommandBuffer(currentFrame), 
                 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(command->getCommandBuffer(currentFrame), 
        sgModel->get_IndexBuffer_handle(), 0, VK_INDEX_TYPE_UINT32);
    
    
    
        // Update descriptor set for both UBO and texture
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
    
        // UBO update
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = sgModel->get_uniformBuffersAt(currentFrame)->get_handle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);
    
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = *descriptorSet->get_handle_ptr_at_index(currentFrame);
        descriptorWrites[0].dstBinding = 0;  // UBO binding
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
    
        // Texture update
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = sgModel->get_texture()->getImageView();
        imageInfo.sampler = sgModel->get_texture()->getSampler();
    
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = *descriptorSet->get_handle_ptr_at_index(currentFrame);
        descriptorWrites[1].dstBinding = 1;  // Texture binding
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
    
        vkUpdateDescriptorSets(device.getDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    
    
    
        vkCmdBindDescriptorSets(command->getCommandBuffer(currentFrame), 
             VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get_PipelineLayout(), 0,
                                1, 
                                descriptorSet->get_handle_ptr_at_index(currentFrame), 
                                0, nullptr);
        vkCmdDrawIndexed(command->getCommandBuffer(currentFrame), 
                        static_cast<uint32_t>(sgModel->getIndicesSize()), 
                        1, 0, 0, 0);  
    }

    VkBuffer vertexBuffers[] = {model->get_VertexBuffer_handle()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command->getCommandBuffer(currentFrame), 
             0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command->getCommandBuffer(currentFrame), 
                 model->get_IndexBuffer_handle(), 0, VK_INDEX_TYPE_UINT32);



    // Update descriptor set for both UBO and texture
    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    // UBO update
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = model->get_uniformBuffersAt(currentFrame)->get_handle();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = *descriptorSet->get_handle_ptr_at_index(currentFrame);
    descriptorWrites[0].dstBinding = 0;  // UBO binding
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    // Texture update
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = model->get_texture()->getImageView();
    imageInfo.sampler = model->get_texture()->getSampler();

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = *descriptorSet->get_handle_ptr_at_index(currentFrame);
    descriptorWrites[1].dstBinding = 1;  // Texture binding
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device.getDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);



    vkCmdBindDescriptorSets(command->getCommandBuffer(currentFrame), 
         VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get_PipelineLayout(), 0,
                            1, 
                            descriptorSet->get_handle_ptr_at_index(currentFrame), 
                            0, nullptr);
    vkCmdDrawIndexed(command->getCommandBuffer(currentFrame), 
                    static_cast<uint32_t>(model->getIndicesSize()), 
                    1, 0, 0, 0);            

    vkCmdEndRenderPass(command->getCommandBuffer(currentFrame));

    command->end(currentFrame);
}

} // namespace Core
