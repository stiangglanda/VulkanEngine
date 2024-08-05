// #include "VulkanBuffer.h"
// #include <GLFW/glfw3.h>
// #include "../../../Application.h"

/* Copyright (c) 2019-2024, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VulkanBuffer.h"


namespace Core
{

VulkanBuffer BufferBuilder::build(VulkanDevice &device) const
{
	return VulkanBuffer(device, *this);
}

std::unique_ptr<VulkanBuffer> BufferBuilder::build_unique(VulkanDevice &device) const
{
	return std::make_unique<VulkanBuffer>(device, *this);
}

VulkanBuffer VulkanBuffer::create_staging_buffer(VulkanDevice &device, VkDeviceSize size, const void *data)
{
	BufferBuilder builder{size};
	builder.with_vma_flags(VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	builder.with_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	VulkanBuffer result(device, builder);
	if (data != nullptr)
	{
		result.update(data, size);
	}
	return result;
}

VulkanBuffer::VulkanBuffer(VulkanDevice                 &device,
               VkDeviceSize                 size,
               VkBufferUsageFlags           buffer_usage,
               VmaMemoryUsage               memory_usage,
               VmaAllocationCreateFlags     flags,
               const std::vector<uint32_t> &queue_family_indices) :
    VulkanBuffer(device,
           BufferBuilder(size)
               .with_usage(buffer_usage)
               .with_vma_usage(memory_usage)
               .with_vma_flags(flags)
               .with_queue_families(queue_family_indices)
               .with_implicit_sharing_mode())
{}

VulkanBuffer::VulkanBuffer(VulkanDevice &device, const BufferBuilder &builder) :
    Allocated{builder.alloc_create_info, VK_NULL_HANDLE, &device}, size(builder.create_info.size)
{
	set_handle(create_buffer(builder.create_info));
	if (!builder.debug_name.empty())
	{
		// set_debug_name(builder.debug_name);//TODO maybe implement debug name in VulkanResource
	}
}

VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept :
    Allocated{std::move(other)},
    size{std::exchange(other.size, {})}
{
}

VulkanBuffer::~VulkanBuffer()
{
	destroy_buffer(get_handle());
}

VkDeviceSize VulkanBuffer::get_size() const
{
	return size;
}

// uint64_t VulkanBuffer::get_device_address() const
// {
// 	VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
// 	buffer_device_address_info.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
// 	buffer_device_address_info.buffer = get_handle();
// 	return vkGetBufferDeviceAddressKHR(get_device().getDevice(), &buffer_device_address_info);
// }

void VulkanBuffer::copyBuffer(std::weak_ptr<VulkanCommandBuffer> command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer;

    if(auto tmp = command.lock())
    {
        commandBuffer=tmp->beginSingleTimeCommands();
    }
    else
    {
        VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
    }

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    if(auto tmp = command.lock())
    {
        tmp->endSingleTimeCommands(commandBuffer);
    }
    else
    {
        VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
    }
}

}        // namespace Core

// namespace Core 
// {

// void VulkanBuffer::Init(VkInstance instance)
// {
//     VE_CORE_INFO("Successful: Init VulkanBuffer");
// }

// void VulkanBuffer::Shutdown(VkInstance instance)
// {
//     VE_CORE_INFO("Shutdown VulkanBuffer");
// }

// void VulkanBuffer::createBuffer(const VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
//                                VkBuffer &buffer, VkDeviceMemory &bufferMemory) // TODO should be in VulkanBuffer
// {
//     VkBufferCreateInfo bufferInfo{};
//     bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//     bufferInfo.size = size;
//     bufferInfo.usage = usage;
//     bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

//     if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
//     {
//         throw std::runtime_error("failed to create buffer!");
//     }

//     VkMemoryRequirements memRequirements;
//     vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memRequirements);
//     VkMemoryAllocateInfo allocInfo{};
//     allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//     allocInfo.allocationSize = memRequirements.size;
//     allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

//     if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
//     {
//         throw std::runtime_error("failed to allocate buffer memory!");
//     }

//     vkBindBufferMemory(device.getDevice(), buffer, bufferMemory, 0);
// }

// void VulkanBuffer::copyBuffer(std::weak_ptr<VulkanCommandBuffer> command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
// {
//     VkCommandBuffer commandBuffer;

//     if(auto tmp = command.lock())
//     {
//         commandBuffer=tmp->beginSingleTimeCommands();
//     }
//     else
//     {
//         VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
//     }

//     VkBufferCopy copyRegion{};
//     copyRegion.size = size;
//     vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

//     if(auto tmp = command.lock())
//     {
//         tmp->endSingleTimeCommands(commandBuffer);
//     }
//     else
//     {
//         VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
//     }
// }


// // AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
// // {
// //     // allocate buffer
// //     VkBufferCreateInfo bufferInfo = {};
// //     bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
// //     bufferInfo.pNext = nullptr;
// //     bufferInfo.size = allocSize;

// //     bufferInfo.usage = usage;

// //     VmaAllocationCreateInfo vmaallocInfo = {};
// //     vmaallocInfo.usage = memoryUsage;
// //     vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
// //     AllocatedBuffer newBuffer;

// //     // allocate the buffer
// //     VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation,
// //                              &newBuffer.info));

// //     return newBuffer;
// // }

// } // namespace Core