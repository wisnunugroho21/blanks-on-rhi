#include "vulkan-backend.hpp"

namespace RHI {
    VulkanCommandBuffer::~VulkanCommandBuffer() {
        VkDevice nativeDevice = this->device->getNative();

        for (auto &&frameBuffer : this->frameBuffers) {
            vkDestroyFramebuffer(nativeDevice, frameBuffer, nullptr);
        }
    }
}