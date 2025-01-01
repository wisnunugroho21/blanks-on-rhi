#include "vulkan-backend.hpp"

namespace RHI {
    VulkanQueue::VulkanQueue(
        QueueDescriptor desc,
        VulkanDevice* d,
        VkQueue q,
        Uint32 familyIndex
    )
    : desc{desc},
      device{d},
      queue{q},
      familyIndex{familyIndex}
    {
        
    }

    std::shared_ptr<QueueAsync> VulkanQueue::submit(std::vector<CommandBuffer*> commandBuffers, 
        std::vector<std::shared_ptr<QueueAsync>> signaled) 
    {

    }
}