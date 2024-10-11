#pragma once
#include "../../../vepch.h"
#include "../../RenderAPI.h"
#include "VulkanDebug.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "camera.h"
#include <memory>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanSwapChain.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VulkanRenderPass.h"
#include "UniformBufferObject.h"
#include "VulkanDescriptorSet.h"
#include "VulkanModel.h"
#include "VulkanSync.h"

struct GLFWwindow;

namespace Core
{

class VulkanAPI : public RenderAPI
{
  public:
    VulkanAPI()
    {
    }
    virtual ~VulkanAPI() = default;

    virtual bool Init() override;
    virtual bool Shutdown() override;
    virtual void Update(float delta) override;
    virtual void Draw() override;
    virtual void OnEvent(Event &e, float delta) override;

  private:
    void createUniformBuffers();//TODO should probobly be in VulkanModel //TODO split UniformBuffers one for per frame and one per model

    void updateUniformBuffer(uint32_t currentImage);//TODO should probobly be in VulkanModel
    void recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex);

#ifdef NDEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = true;
#endif

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    const int MAX_FRAMES_IN_FLIGHT = 2;

    VulkanInstance instance; // Vulkan library handle
    VulkanDebug vkDebug;     // Vulkan debug output handle
    VulkanSurface surface;   // Vulkan window surface
    VulkanDevice device;     // Vulkan device for commands // GPU chosen as the default device
    std::shared_ptr<VulkanImage> texture;//TODO should probobly be in VulkanModel
    std::shared_ptr<VulkanCommandBuffer> command;

    Camera Cam;

    VulkanSwapChain swapChain;

    std::unique_ptr<VulkanRenderPass> renderPass;
    std::unique_ptr<VulkanDescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<VulkanPipeline> graphicsPipeline;

    std::unique_ptr<VulkanModel> model;

    std::vector<std::unique_ptr<VulkanBuffer>> uniformBuffers;//TODO should probobly be in VulkanModel
    std::vector<void *> uniformBuffersMapped;//TODO should probobly be in VulkanModel
    std::unique_ptr<VulkanDescriptorSet> descriptorSet;//TODO should probobly be in VulkanModel

    std::unique_ptr<VulkanSync> sync;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;
};

} // namespace Core
