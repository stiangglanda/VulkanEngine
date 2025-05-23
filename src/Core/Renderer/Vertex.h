#pragma once
#include <glm/glm.hpp>
#include <volk.h>
#include <glm/gtx/hash.hpp>
#include "../vepch.h"

namespace Core
{

struct Vertex//TODO should not be in Vulkan folder and should not be vulkan specific
{
    glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
    glm::vec4 color; // can also be tangent
    
    //glm::vec3 pos;
    //glm::vec3 color;
    //glm::vec2 texCoord;

    //static VkVertexInputBindingDescription getBindingDescription()
    //{
    //    VkVertexInputBindingDescription bindingDescription{};
    //    bindingDescription.binding = 0;
    //    bindingDescription.stride = sizeof(Vertex);
    //    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    //    return bindingDescription;
    //}

    //static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    //{
    //    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    //    attributeDescriptions[0].binding = 0;
    //    attributeDescriptions[0].location = 0;
    //    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    //    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    //    attributeDescriptions[1].binding = 0;
    //    attributeDescriptions[1].location = 1;
    //    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    //    attributeDescriptions[1].offset = offsetof(Vertex, color);

    //    attributeDescriptions[2].binding = 0;
    //    attributeDescriptions[2].location = 2;
    //    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    //    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    //    return attributeDescriptions;
    //}

    bool operator==(const Vertex &other) const
    {
        return position == other.position && color == other.color && normal == other.normal && uv_x == other.uv_x &&
               uv_y == other.uv_y;
    }
};

}
namespace std
{
template <> struct hash<Core::Vertex>
{
    size_t operator()(Core::Vertex const &vertex) const
    {
        return ((((hash<glm::vec3>()(vertex.position) 
               ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) 
               ^ (hash<glm::vec4>()(vertex.color) << 1)) >> 1) 
               ^ ((hash<float>()(vertex.uv_x) 
               ^ (hash<float>()(vertex.uv_y) << 1)) >> 1);
    }
};
} // namespace std