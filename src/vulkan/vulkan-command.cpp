#include "vulkan-backend.hpp"

namespace RHI {
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

    void VulkanCommandEncoder::activateTextureBarrier(PipelineStageFlags srcStage, PipelineStageFlags dstStage, TextureBarrier desc) {
        VkImageMemoryBarrier imageBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = convertTextureAccessIntoVulkan(srcStage, desc.srcAccess, desc.view->getTexture()),
            .dstAccessMask = convertTextureAccessIntoVulkan(dstStage, desc.dstAccess, desc.view->getTexture()),
            .oldLayout = convertTextureStateIntoVulkan(desc.srcState),
            .newLayout = convertTextureStateIntoVulkan(desc.dstState),
            .subresourceRange.aspectMask = convertAspectIntoVulkan(desc.view->getDesc().subresource.aspect),
            .subresourceRange.baseArrayLayer = desc.view->getDesc().subresource.baseArrayLayer,
            .subresourceRange.layerCount = desc.view->getDesc().subresource.arrayLayerCount,
            .subresourceRange.baseMipLevel = desc.view->getDesc().subresource.baseMipLevel,
            .subresourceRange.levelCount = desc.view->getDesc().subresource.mipLevelCount
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

        dynamic_cast<VulkanTexture*>(desc.view->getTexture())->setState(desc.dstState);
    }

    void VulkanCommandEncoder::copyBufferToBuffer(Buffer* source, Uint64 sourceOffset, Buffer* destination, Uint64 destinationOffset, Uint64 size) {
        VkBufferCopy copyRegion {
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

    void VulkanCommandEncoder::copyBufferToTexture(CopyBuffer source, CopyTexture destination, Extent3D copySize) {
        VkBufferImageCopy copyRegion {
            .bufferOffset = source.offset,
            .bufferRowLength = source.bytesPerRow,
            .bufferImageHeight = source.rowsPerImage * source.bytesPerRow,
            .imageSubresource = {
                .aspectMask = convertAspectIntoVulkan(destination.view->getDesc().subresource.aspect),
                .mipLevel = destination.view->getDesc().subresource.mipLevelCount,
                .baseArrayLayer = destination.view->getDesc().subresource.baseArrayLayer,
                .layerCount = destination.view->getDesc().subresource.arrayLayerCount
            },
            .imageOffset = {
                .x = static_cast<Int32>(destination.origin.x),
                .y = static_cast<Int32>(destination.origin.y),
                .z = static_cast<Int32>(destination.origin.z)
            },
            .imageExtent = {
                .width = copySize.width,
                .height = copySize.height,
                .depth = copySize.depthOrArrayLayers
            }
        };

        vkCmdCopyBufferToImage(
            this->commandBuffer,
            dynamic_cast<VulkanBuffer*>(source.buffer)->getNative(),
            dynamic_cast<VulkanTexture*>(destination.view->getTexture())->getNative(),
            convertTextureStateIntoVulkan(destination.view->getTexture()->getState()),
            1,
            &copyRegion
        );
    }

    void VulkanCommandEncoder::copyTextureToBuffer(CopyTexture source, CopyBuffer destination, Extent3D copySize) {
        VkBufferImageCopy copyRegion {
            .bufferOffset = destination.offset,
            .bufferRowLength = destination.bytesPerRow,
            .bufferImageHeight = destination.rowsPerImage * destination.bytesPerRow,
            .imageSubresource = {
                .aspectMask = convertAspectIntoVulkan(source.view->getDesc().subresource.aspect),
                .mipLevel = source.view->getDesc().subresource.mipLevelCount,
                .baseArrayLayer = source.view->getDesc().subresource.baseArrayLayer,
                .layerCount = source.view->getDesc().subresource.arrayLayerCount
            },
            .imageOffset = {
                .x = static_cast<Int32>(source.origin.x),
                .y = static_cast<Int32>(source.origin.y),
                .z = static_cast<Int32>(source.origin.z)
            },
            .imageExtent = {
                .width = copySize.width,
                .height = copySize.height,
                .depth = copySize.depthOrArrayLayers
            }
        };

        vkCmdCopyImageToBuffer(
            this->commandBuffer,
            dynamic_cast<VulkanTexture*>(source.view->getTexture())->getNative(),
            convertTextureStateIntoVulkan(source.view->getTexture()->getState()),
            dynamic_cast<VulkanBuffer*>(destination.buffer)->getNative(),
            1,
            &copyRegion
        );
    }

    void VulkanCommandEncoder::copyTextureToTexture(CopyTexture source, CopyTexture destination, Extent3D copySize) {
        VkImageCopy copyRegion {
            .srcSubresource = {
                .aspectMask = convertAspectIntoVulkan(source.view->getDesc().subresource.aspect),
                .mipLevel = source.view->getDesc().subresource.mipLevelCount,
                .baseArrayLayer = source.view->getDesc().subresource.baseArrayLayer,
                .layerCount = source.view->getDesc().subresource.arrayLayerCount
            },
            .srcOffset = {
                .x = source.origin.x,
                .y = source.origin.y,
                .z = source.origin.z
            },
            .dstSubresource = {
                .aspectMask = convertAspectIntoVulkan(destination.view->getDesc().subresource.aspect),
                .mipLevel = destination.view->getDesc().subresource.mipLevelCount,
                .baseArrayLayer = destination.view->getDesc().subresource.baseArrayLayer,
                .layerCount = destination.view->getDesc().subresource.arrayLayerCount
            },
            .dstOffset = {
                .x = destination.origin.x,
                .y = destination.origin.y,
                .z = destination.origin.z
            },
            .extent = {
                .width = copySize.width,
                .height = copySize.height,
                .depth = copySize.depthOrArrayLayers
            }
        };

        vkCmdCopyImage(
            this->commandBuffer,
            dynamic_cast<VulkanTexture*>(source.view->getTexture())->getNative(),
            convertTextureStateIntoVulkan(source.view->getTexture()->getState()),
            dynamic_cast<VulkanTexture*>(destination.view->getTexture())->getNative(),
            convertTextureStateIntoVulkan(destination.view->getTexture()->getState()),
            1,
            &copyRegion
        );
    }

    void VulkanCommandEncoder::fillBuffer(Uint32 data, Buffer* buffer, Uint64 size, Uint64 offset) {
        vkCmdFillBuffer(
            this->commandBuffer,
            dynamic_cast<VulkanBuffer*>(buffer)->getNative(),
            size == ULLONG_MAX ? VK_WHOLE_SIZE : size,
            offset,
            data
        );
    }

    void VulkanCommandEncoder::resolveQuerySet(QuerySet querySet, Uint32 firstQuery, Uint32 queryCount, Buffer* destination, Uint64 destinationOffset) {

    }

    void VulkanCommandEncoder::finish() {
        vkEndCommandBuffer(this->commandBuffer);
    }

    VulkanCommandEncoder::~VulkanCommandEncoder() {

    }
}