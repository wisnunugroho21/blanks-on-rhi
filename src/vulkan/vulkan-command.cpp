#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<CommandEncoder> VulkanDevice::beginCommandEncoder(CommandEncoderDescriptor desc) {
        VkCommandBufferAllocateInfo commandBufferInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = this->commandPools[desc.queueType],
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer commandBuffer;
        if (vkAllocateCommandBuffers(this->device, &commandBufferInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }

        /* VulkanCommandEncoder(
            CommandEncoderDescriptor desc,
            VulkanDevice* d,
            VkCommandBuffer c
        ) */

        return std::make_shared<VulkanCommandEncoder>(desc, this, commandBuffer);
    }

    void VulkanCommandEncoder::activatePipelineBarrier(ShaderStage srcStage,ShaderStage dstStage) {

    }

    void VulkanCommandEncoder::activateBufferBarrier(ShaderStage srcStage,ShaderStage dstStage,BufferBarrier desc) {

    }

    void VulkanCommandEncoder::activateImageBarrier(ShaderStage srcStage, ShaderStage dstStage, ImageBarrier desc) {

    }

    void VulkanCommandEncoder::copyBufferToBuffer(Buffer* source, Uint64 sourceOffset, Buffer* destination, Uint64 destinationOffset, Uint64 size) {

    }

    void VulkanCommandEncoder::copyBufferToTexture(ImageCopyBuffer source, ImageCopyTexture destination, Extent3D copySize) {

    }

    void VulkanCommandEncoder::copyTextureToBuffer(ImageCopyTexture source, ImageCopyBuffer destination, Extent3D copySize) {

    }

    void VulkanCommandEncoder::copyTextureToTexture(ImageCopyTexture source, ImageCopyTexture destination, Extent3D copySize) {

    }

    void VulkanCommandEncoder::clearBuffer(Buffer* buffer, Uint64 size, Uint64 offset) {

    }

    void VulkanCommandEncoder::resolveQuerySet(QuerySet querySet, Uint32 firstQuery, Uint32 queryCount, Buffer* destination, Uint64 destinationOffset) {

    }

    void VulkanCommandEncoder::finish() {

    }

    VulkanCommandEncoder::~VulkanCommandEncoder() {

    }
}