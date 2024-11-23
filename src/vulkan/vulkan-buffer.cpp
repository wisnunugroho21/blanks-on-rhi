#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<Buffer> VulkanDevice::createBuffer(BufferDescriptor desc) {
        VkBufferCreateInfo bufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = desc.size,
            .usage = convertBufferUsageIntoVulkan(desc.usage)
        };

        VmaAllocationCreateInfo allocInfo = {
            .usage = convertBufferLocationIntoVulkan(desc.location),
            .flags = convertToAllocationFlag(desc.usage, desc.location)
        };

        VkBuffer buffer;
        VmaAllocation memoryAllocation;

        if (vmaCreateBuffer(this->memoryAllocator, &bufferInfo, &allocInfo, &buffer, &memoryAllocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer!");
        }

        return std::make_shared<VulkanBuffer>(desc, this, buffer, memoryAllocation);
    }

    void VulkanBuffer::insertData(void* pointerData, Uint64 size, Uint64 offset) {
        if (vmaCopyMemoryToAllocation(this->device->getMemoryAllocator(), pointerData, this->memoryAllocation, offset, size) != VK_SUCCESS) {
            throw std::runtime_error("Failed to insert data!");
        }
    }

    void VulkanBuffer::takeData(void* pointerData, Uint64 size, Uint64 offset) {
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

    void VulkanBuffer::flush(Uint64 size, Uint64 offset) {
        if (vmaFlushAllocation(this->device->getMemoryAllocator(), this->memoryAllocation, offset, size) != VK_SUCCESS) {
            throw std::runtime_error("Failed to flush buffer!");
        }
    }

    void VulkanBuffer::invalidate(Uint64 size, Uint64 offset) {
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