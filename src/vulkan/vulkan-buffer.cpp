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

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = vulkanMemoryUsage;
        allocInfo.flags = vulkanAllocationFlag;

        if (vmaCreateBuffer(this->memoryAllocator, &bufferInfo, &allocInfo, &buffer,
                            &memoryAllocation, nullptr) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create buffer!");
        }

        return std::make_shared<VulkanBuffer>(desc, buffer, memoryAllocation);
    }

    void VulkanBuffer::insertData(Uint64 size = ULLONG_MAX, Uint64 offset = 0, void* pointerData) {
        if (vmaCopyMemoryToAllocation(this->device->getMemoryAllocator(), pointerData, this->memoryAllocation, offset, size) != VK_SUCCESS) {
            throw std::runtime_error("Failed to insert data!");
        }
    }

    void VulkanBuffer::takeData(Uint64 size = ULLONG_MAX, Uint64 offset = 0, void* pointerData) {
        if (vmaCopyAllocationToMemory(this->device->getMemoryAllocator(), this->memoryAllocation, offset, pointerData, size) != VK_SUCCESS) {
            throw std::runtime_error("Failed to take data!");
        }
    }

    void* VulkanBuffer::map() {
        if (this->mapState == BufferMapState::eMapped) {
            throw std::runtime_error("Buffer has already mapped! Just get the mapped pointer by using getMapped() function!");
        }

        if (vmaMapMemory(this->device->getMemoryAllocator(), this->memoryAllocation, &this->mapped) != VK_SUCCESS) {
            throw std::runtime_error("Failed to map buffer!");
        }

        this->mapState = BufferMapState::eMapped;
        return this->mapped;
    }

    void VulkanBuffer::unmap() {
        if (this->mapState == BufferMapState::eUnmapped) {
            throw std::runtime_error("Buffer is not mapped yet! You must map it first by using map() function!");
        }

        vmaUnmapMemory(this->device->getMemoryAllocator(), this->memoryAllocation);
        this->mapState = BufferMapState::eUnmapped;
    }

    void VulkanBuffer::flush(Uint64 size = ULLONG_MAX, Uint64 offset = 0) {
        if (vmaFlushAllocation(this->device->getMemoryAllocator(), this->memoryAllocation, offset, size) != VK_SUCCESS) {
            throw std::runtime_error("Failed to flush buffer!");
        }
    }

    void VulkanBuffer::invalidate(Uint64 size = ULLONG_MAX, Uint64 offset = 0) {
        if (vmaInvalidateAllocation(this->device->getMemoryAllocator(), this->memoryAllocation, offset, size) != VK_SUCCESS) {
            throw std::runtime_error("Failed to invalidate buffer!");
        }
    }

    VulkanBuffer::~VulkanBuffer() {
        if (this->mapState == BufferMapState::eMapped) {
            this->unmap();
        }
        
        vmaDestroyBuffer(this->device->getMemoryAllocator(), this->buffer, this->memoryAllocation);
    }
}