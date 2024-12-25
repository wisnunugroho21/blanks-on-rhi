#include "vulkan-backend.hpp"

namespace RHI {
    void VulkanQueue::submit(std::vector<CommandEncoder*> commandBuffers) {
        std::vector<VkCommandBuffer> vulkanCommandBuffers;

        for (auto &&commandBuffer : commandBuffers) {
            vulkanCommandBuffers.emplace_back(dynamic_cast<VulkanCommandEncoder*>(commandBuffer)->getNative());
        }

        VkSubmitInfo commandSubmit{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = static_cast<uint32_t>(vulkanCommandBuffers.size()),
            .pCommandBuffers = vulkanCommandBuffers.data(),
        };
    }
}