#pragma once

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include "../rhi.hpp"

namespace RHI {
    class VulkanBuffer : public Buffer {
    public:
        VulkanBuffer(
            BufferDescriptor desc,
            VkBuffer b,
            VmaAllocation ma
        )
        : Buffer(desc), 
          buffer{b},
          memoryAllocation{ma}
        {}

    private:
        VkBuffer buffer;
        VmaAllocation memoryAllocation;
    };

    class VulkanQueue : public Queue {
    public:
        VulkanQueue(
            QueueDescriptor desc,
            VkQueue q
        )
        : Queue(desc),
          queue{q}
        {}

    void submit(std::vector<CommandEncoder*> commandBuffers) override;

    private:
        VkQueue queue;
    };

    class VulkanDevice : public Device {
    public:
        VulkanDevice(
            DeviceDescriptor desc,
            VkPhysicalDevice pd, 
            VkDevice d,
            VkDebugUtilsMessengerEXT debugMessenger,
            VkPhysicalDeviceProperties dp,
            VmaAllocator vma,
            std::vector<std::shared_ptr<Queue>> q            
        )
        : Device(desc),
          physicalDevice{pd}, 
          device{d},
          deviceProperties{dp},
          memoryAllocator{vma},
          queues{q}
        {

        }

        std::shared_ptr<Buffer> createBuffer(BufferDescriptor descriptor);
        
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDeviceProperties deviceProperties;
        VmaAllocator memoryAllocator;

        std::vector<std::shared_ptr<Queue>> queues;
    };

    class VulkanFactory {
        static std::shared_ptr<Device> createDevice(DeviceDescriptor desc);

        static void createInstance(DeviceDescriptor desc, VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger);
        static void pickPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice, VkPhysicalDeviceProperties* deviceProperties);
        static void createLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice* device, std::vector<std::shared_ptr<Queue>>* queues);
        static void createMemoryAllocator(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator* memoryAllocator);
    };
}