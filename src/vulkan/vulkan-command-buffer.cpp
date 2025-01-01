#include "vulkan-backend.hpp"

namespace RHI {
    VulkanCommandBuffer::~VulkanCommandBuffer() {
        VkDevice nativeDevice = this->device->getNative();

        for (auto &&frameBuffer : this->frameBuffers) {
            vkDestroyFramebuffer(nativeDevice, frameBuffer, nullptr);
        }
        
        vkFreeDescriptorSets(this->device->getNative(), 
            this->device->getDescriptorPool(), 
            static_cast<Uint32>(this->descSets.size()), 
            this->descSets.data()
        );

        vkFreeCommandBuffers(
            this->device->getNative(), 
            this->device->getCommandPools()[this->desc.queueType], 
            1, 
            &this->commandBuffer
        );
    }
}