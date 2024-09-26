#pragma once
#include <glm/glm.hpp>

namespace Core
{

    struct UniformBufferObject//TODO should not be in Vulkan folder and should not be vulkan specific
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
}