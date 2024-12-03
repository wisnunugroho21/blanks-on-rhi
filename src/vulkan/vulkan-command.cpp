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

        return std::make_shared<VulkanCommandEncoder>(desc, this, commandBuffer);
    }

    void VulkanCommandEncoder::activatePipelineBarrier(PipelinerStageFlags srcStage,PipelinerStageFlags dstStage) {
        vkCmdPipelineBarrier(
            this->commandBuffer,
            convertPipelineStageIntoVulkan(srcStage),
            convertPipelineStageIntoVulkan(dstStage),
            0,
            0,
            nullptr,
            0,
            nullptr,
            0,
            nullptr
        );
    }

    void VulkanCommandEncoder::activateBufferBarrier(PipelinerStageFlags srcStage,PipelinerStageFlags dstStage, BufferBarrier desc) {
        VkBufferMemoryBarrier bufferBarrier{
          .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
          .srcAccessMask = convertBufferAccessIntoVulkan(srcStage, desc.srcAccess, desc.buffer),
          .dstAccessMask = convertBufferAccessIntoVulkan(dstStage, desc.dstAccess, desc.buffer),
          .buffer = dynamic_cast<VulkanBuffer*>(desc.buffer)->getNative(),
          .offset = desc.offset,
          .size = desc.size
        };

        vkCmdPipelineBarrier(
            this->commandBuffer,
            convertPipelineStageIntoVulkan(srcStage),
            convertPipelineStageIntoVulkan(dstStage),
            0,
            0,
            nullptr,
            1,
            &bufferBarrier,
            0,
            nullptr
        );
    }

    void VulkanCommandEncoder::activateTextureBarrier(PipelinerStageFlags srcStage, PipelinerStageFlags dstStage, ImageBarrier desc) {
        VkImageMemoryBarrier imageBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = convertTextureAccessIntoVulkan(srcStage, desc.srcAccess, desc.textureView->getTexture()),
            .dstAccessMask = convertTextureAccessIntoVulkan(dstStage, desc.dstAccess, desc.textureView->getTexture()),
            .oldLayout = convertTextureStateIntoVulkan(desc.srcState),
            .newLayout = convertTextureStateIntoVulkan(desc.dstState),
            .subresourceRange.aspectMask = convertAspectIntoVulkan(desc.textureView->getDesc().subresource.aspect),
            .subresourceRange.baseArrayLayer = desc.textureView->getDesc().subresource.baseArrayLayer,
            .subresourceRange.layerCount = desc.textureView->getDesc().subresource.arrayLayerCount,
            .subresourceRange.baseMipLevel = desc.textureView->getDesc().subresource.baseMipLevel,
            .subresourceRange.levelCount = desc.textureView->getDesc().subresource.mipLevelCount
        };

        vkCmdPipelineBarrier(
            this->commandBuffer,
            convertPipelineStageIntoVulkan(srcStage),
            convertPipelineStageIntoVulkan(dstStage),
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageBarrier
        );
    }

    void VulkanCommandEncoder::copyBufferToBuffer(Buffer* source, Uint64 sourceOffset, Buffer* destination, Uint64 destinationOffset, Uint64 size) {
        VkBufferCopy copyRegion{
            .srcOffset = sourceOffset,
            .srcOffset = destinationOffset,
            .size = size
        };

        vkCmdCopyBuffer(
            this->commandBuffer,
            dynamic_cast<VulkanBuffer*>(source)->getNative(),
            dynamic_cast<VulkanBuffer*>(destination)->getNative(),
            1,
            &copyRegion
        );
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