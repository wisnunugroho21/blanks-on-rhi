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