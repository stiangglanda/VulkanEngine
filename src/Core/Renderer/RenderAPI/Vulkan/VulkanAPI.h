#pragma once
#include "../../../vepch.h"
#include "../../RenderAPI.h"
#include "VulkanDebug.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanPipeline.h"
#include "VulkanSync.h"
#include "VulkanModel.h"
#include "camera.h"
#include "../../VoxelModel.h"

namespace Core
{

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

class VulkanAPI : public RenderAPI
{
public:
    VulkanAPI()=default;
    ~VulkanAPI()=default;

    bool Init() override;
    bool Shutdown() override;

    void OnEvent(Event &e, float delta) override;
    void Update(float delta) override;
    void Draw() override;

    std::shared_ptr<Model> LoadModel(const std::string model_path, const std::string texture_path) override;
    std::shared_ptr<VoxelModel> LoadVoxelModel(const std::string model_path) override;

private:
    void recordCommandBuffer(uint32_t currentImage, uint32_t imageIndex);

    std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    VulkanInstance instance;  // Vulkan library handle
    VulkanDebug vkDebug;     // Vulkan debug output handle
    VulkanSurface surface;   // Vulkan window surface
    VulkanDevice device;     // Vulkan device for commands
    std::shared_ptr<VulkanCommandBuffer> command;

    Camera Cam;

    VulkanSwapChain swapChain;

    std::unique_ptr<VulkanRenderPass> renderPass;
    std::unique_ptr<VulkanDescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<VulkanPipeline> graphicsPipeline;

    std::unique_ptr<VulkanModel> model;

    std::unique_ptr<VulkanSync> sync;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;
};

} // namespace Core
