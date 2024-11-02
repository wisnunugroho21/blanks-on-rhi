#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    std::shared_ptr<Buffer> VulkanDevice::createBuffer(BufferDescriptor descriptor) {
        VkBuffer buffer;
        VmaAllocation memoryAllocation;

        VkBufferUsageFlags vulkanBufferFlags = 0;

        if (descriptor.usage & std::to_underlying(BufferUsage::eCopySrc)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        } 
        
        if (descriptor.usage & std::to_underlying(BufferUsage::eCopyDst)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        } 
        
        if (descriptor.usage & std::to_underlying(BufferUsage::eIndex)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        } 
        
        if (descriptor.usage & std::to_underlying(BufferUsage::eVertex)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        } 
        
        if (descriptor.usage & std::to_underlying(BufferUsage::eUniform)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        } 
        
        if (descriptor.usage & std::to_underlying(BufferUsage::eStorage)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        } 
        
        if (descriptor.usage & std::to_underlying(BufferUsage::eIndirect)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        VmaMemoryUsage vulkanMemoryUsage;
        switch (descriptor.location) {
            case BufferLocation::eDeviceLocal:
                vulkanMemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                break;
            
            case BufferLocation::eHost:
                vulkanMemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
                break;
            
            default:
                vulkanMemoryUsage = VMA_MEMORY_USAGE_AUTO;
                break;
        }

        VmaAllocationCreateFlags vulkanAllocationFlag{};
        if (descriptor.usage & std::to_underlying(BufferUsage::eCopySrc)
            && descriptor.location == BufferLocation::eHost)
        {
            vulkanAllocationFlag = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        } 
        
        else if (descriptor.usage & std::to_underlying(BufferUsage::eCopyDst)
                && descriptor.location == BufferLocation::eHost)
        {
            vulkanAllocationFlag = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        } 
        
        else if (descriptor.location == BufferLocation::eDeviceLocal)  {
            vulkanAllocationFlag = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        }

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = descriptor.size;
        bufferInfo.usage = vulkanBufferFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = vulkanMemoryUsage;
        allocInfo.flags = vulkanAllocationFlag;

        if (vmaCreateBuffer(this->memoryAllocator, &bufferInfo, &allocInfo, &buffer,
                            &memoryAllocation, nullptr) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create buffer!");
        }

        return std::make_shared<VulkanBuffer>(desc, buffer, memoryAllocation);
    }
}