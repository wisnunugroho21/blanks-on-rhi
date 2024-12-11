#include "vulkan-backend.hpp"

namespace RHI {
    void VulkanCommandEncoder::copyBufferToBuffer(Buffer* source, Uint64 sourceOffset, Buffer* destination, Uint64 destinationOffset, Uint64 size) {
        VkBufferCopy copyRegion{
            .srcOffset = sourceOffset,
            .dstOffset = destinationOffset,
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

    void VulkanCommandEncoder::activatePipelineBarrier(PipelineStageFlags srcStage,PipelineStageFlags dstStage) {
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

    void VulkanCommandEncoder::activateBufferBarrier(PipelineStageFlags srcStage,PipelineStageFlags dstStage, BufferBarrier desc) {
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

    void VulkanCommandEncoder::activateTextureBarrier(PipelineStageFlags srcStage, PipelineStageFlags dstStage, ImageBarrier desc) {
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
        vkEndCommandBuffer(this->commandBuffer);
    }

    VulkanCommandEncoder::~VulkanCommandEncoder() {

    }
}