#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "../rhi.hpp"

namespace RHI {
    class VulkanDevice : public Device {
    public:
        VulkanDevice(
            const DeviceDescriptor& desc,
            const VkPhysicalDevice& pd, 
            const VkDevice& d,
            const VkPhysicalDeviceProperties& dp,
            const VmaAllocator& vma            
        )
        : Device(desc),
          physicalDevice{pd}, 
          device{d},
          deviceProperties{dp},
          memoryAllocator{vma}
        {

        }        
        
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDeviceProperties deviceProperties;
        VmaAllocator memoryAllocator;
    };

    class VulkanFactory {
        static std::shared_ptr<Device> createDevice();

        void createInstance(const DeviceDescriptor& desc, VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger);
        void pickPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice, VkPhysicalDeviceProperties* deviceProperties);
        void createLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice* device, std::vector<std::shared_ptr<Queue>>* queues);
        void createMemoryAllocator(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator* memoryAllocator);
    };
}