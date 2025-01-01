#include "vulkan-backend.hpp"

#include <iostream>
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace RHI {
    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t computeFamily;
        uint32_t transferFamily;

        uint32_t graphicsCount;
        uint32_t computeCount;
        uint32_t transferCount;

        bool graphicsFamilyHasValue = false;
        bool computeFamilyHasValue = false;
        bool transferFamilyHasValue = false;

        bool isComplete() const { return graphicsFamilyHasValue && computeFamilyHasValue && transferFamilyHasValue; }
    };

    std::vector<const char *> getGLFWExtensions(bool enableDebug) {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        return extensions;
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices{};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsCount = queueFamilies[i].queueCount;
                indices.graphicsFamilyHasValue = true;
            }

            if (indices.graphicsFamilyHasValue) {
                break;
            }
        }

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (indices.graphicsFamily == i) {
                continue;
            }

            if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeFamily = i;
                indices.computeCount = queueFamilies[i].queueCount;
                indices.computeFamilyHasValue = true;

                break;
            }
        }

        if (!indices.computeFamilyHasValue) {
            if (queueFamilies[indices.graphicsFamily].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeFamily = indices.graphicsFamily;
                indices.computeCount = indices.graphicsCount;
                indices.computeFamilyHasValue = true;
            }
        }

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (indices.graphicsFamily == i || indices.computeFamily == i) {
                continue;
            }

            if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indices.transferFamily = i;
                indices.transferCount = queueFamilies[i].queueCount;
                indices.transferFamilyHasValue = true;
            }

            if (indices.transferFamilyHasValue) {
                break;
            }
        }

        if (!indices.transferFamilyHasValue) {
            if (queueFamilies[indices.computeFamily].queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indices.transferFamily = indices.computeFamily;
                indices.transferCount = indices.computeCount;
                indices.transferFamilyHasValue = true;
            }

            else if (queueFamilies[indices.graphicsFamily].queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indices.transferFamily = indices.graphicsFamily;
                indices.transferCount = indices.graphicsCount;
                indices.transferFamilyHasValue = true;
            }
        }

        return indices;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
    ) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger
    ) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT"
        );

        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator
    ) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, 
            "vkDestroyDebugUtilsMessengerEXT"
        );

        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void VulkanDevice::createInstance(DeviceDescriptor desc, VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger) {
        VkApplicationInfo appInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_2
        };

        VkInstanceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo
        };

        std::vector<const char *> extensions;

#ifdef __APPLE__
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  
        
        if (desc.enableDebug)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            std::vector<const char *> layers = { "VK_LAYER_KHRONOS_validation" };

            createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
            createInfo.ppEnabledLayerNames = layers.data();

            VkDebugUtilsMessengerCreateInfoEXT debugInfo;

            debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

            debugInfo.pfnUserCallback = debugCallback;
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugInfo;
            
            if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create instance!");
            }

            if (CreateDebugUtilsMessengerEXT(*instance, &debugInfo, nullptr, debugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("Failed to set up debug messenger!");
            }            
        } else {
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;

            if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create instance!");
            }
        }
    }

    void VulkanDevice::pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *selectedPhysicalDevice, VkPhysicalDeviceProperties *deviceProperties) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        std::cout << "Device count: " << deviceCount << std::endl;
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        for (auto &&physicalDevice : physicalDevices) {
            vkGetPhysicalDeviceProperties(physicalDevice, deviceProperties);
            if (deviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                *selectedPhysicalDevice = physicalDevice;
                return;
            }
        }

        *selectedPhysicalDevice = physicalDevices[physicalDevices.size() - 1];
    }

    void VulkanDevice::createLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice *device, 
        std::map<QueueType, std::vector<std::shared_ptr<Queue>>> *queues,
        std::map<QueueType, VkCommandPool> *commandPools) 
    {
        VkDeviceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO
        };

        QueueFamilyIndices familyIndices = findQueueFamilies(physicalDevice);
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = familyIndices.graphicsFamily,
            .queueCount = familyIndices.graphicsCount,
            .pQueuePriorities = &queuePriority
        };

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {queueCreateInfo};

        if (familyIndices.computeFamily != familyIndices.transferFamily
            && familyIndices.transferFamily != familyIndices.graphicsFamily
            && familyIndices.computeFamily != familyIndices.graphicsFamily) 
        {
            queueCreateInfo.queueFamilyIndex = familyIndices.computeFamily;
            queueCreateInfo.queueCount = familyIndices.computeCount;

            queueCreateInfos.emplace_back(queueCreateInfo);

            // ======

            queueCreateInfo.queueFamilyIndex = familyIndices.transferFamily;
            queueCreateInfo.queueCount = familyIndices.transferCount;

            queueCreateInfos.emplace_back(queueCreateInfo);
        } else if (familyIndices.computeFamily == familyIndices.transferFamily
                   && familyIndices.computeFamily != familyIndices.graphicsFamily) 
        {
            queueCreateInfo.queueFamilyIndex = familyIndices.computeFamily;
            queueCreateInfo.queueCount = familyIndices.computeCount;

            queueCreateInfos.emplace_back(queueCreateInfo);
        } else if (familyIndices.computeFamily != familyIndices.transferFamily
                   && familyIndices.computeFamily == familyIndices.graphicsFamily) 
        {
            queueCreateInfo.queueFamilyIndex = familyIndices.transferFamily;
            queueCreateInfo.queueCount = familyIndices.transferCount;

            queueCreateInfos.emplace_back(queueCreateInfo);
        } else if (familyIndices.computeFamily != familyIndices.transferFamily
                   && familyIndices.transferFamily == familyIndices.graphicsFamily) 
        {
            queueCreateInfo.queueFamilyIndex = familyIndices.computeFamily;
            queueCreateInfo.queueCount = familyIndices.computeCount;

            queueCreateInfos.emplace_back(queueCreateInfo);
        }

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        VkPhysicalDeviceFeatures deviceFeatures{};
        createInfo.pEnabledFeatures = &deviceFeatures;

        std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef __APPLE__
        deviceExtensions.emplace_back("VK_KHR_portability_subset");
#endif

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        queues->clear();

        for (uint32_t i = 0; i < familyIndices.graphicsCount; i++) {
            VkQueue vulkanQueue;
            vkGetDeviceQueue(*device, familyIndices.graphicsFamily, i, &vulkanQueue);

            QueueDescriptor desc{
                .type = QueueType::Graphic,
                .index = i
            };

            (*queues)[QueueType::Graphic].push_back(std::make_shared<VulkanQueue>(desc, this, vulkanQueue, familyIndices.graphicsFamily));
        }

        for (uint32_t i = 0; i < familyIndices.computeCount; i++) {
            VkQueue vulkanQueue;
            vkGetDeviceQueue(*device, familyIndices.computeFamily, i, &vulkanQueue);

            QueueDescriptor desc{
                .type = QueueType::Compute,
                .index = i
            };

            (*queues)[QueueType::Compute].push_back(std::make_shared<VulkanQueue>(desc, this, vulkanQueue, familyIndices.computeFamily));
        }

        for (uint32_t i = 0; i < familyIndices.transferCount; i++) {
            VkQueue vulkanQueue;
            vkGetDeviceQueue(*device, familyIndices.transferFamily, i, &vulkanQueue);

            QueueDescriptor desc{
                .type = QueueType::Transfer,
                .index = i
            };

            (*queues)[QueueType::Transfer].push_back(std::make_shared<VulkanQueue>(desc, this, vulkanQueue, familyIndices.transferFamily));
        }

        VkCommandPoolCreateInfo commandPoolInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = familyIndices.graphicsFamily
        };

        VkCommandPool commandPool;
        if (vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphic command pool!"); 
        }

        (*commandPools)[QueueType::Graphic] = commandPool;

        if (familyIndices.computeFamily != familyIndices.transferFamily
            && familyIndices.transferFamily != familyIndices.graphicsFamily
            && familyIndices.computeFamily != familyIndices.graphicsFamily) 
        {
            commandPoolInfo.queueFamilyIndex = familyIndices.computeFamily;

            if (vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create compute command pool!"); 
            }

            (*commandPools)[QueueType::Compute] = commandPool;

            // ======

            commandPoolInfo.queueFamilyIndex = familyIndices.transferFamily;

            if (vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create transfer command pool!"); 
            }

            (*commandPools)[QueueType::Transfer] = commandPool;
        } else if (familyIndices.computeFamily == familyIndices.transferFamily
                   && familyIndices.computeFamily != familyIndices.graphicsFamily) 
        {
            commandPoolInfo.queueFamilyIndex = familyIndices.computeFamily;

            if (vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create compute command pool!"); 
            }

            (*commandPools)[QueueType::Compute] = commandPool;
        } else if (familyIndices.computeFamily != familyIndices.transferFamily
                   && familyIndices.computeFamily == familyIndices.graphicsFamily) 
        {
            commandPoolInfo.queueFamilyIndex = familyIndices.transferFamily;

            if (vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create transfer command pool!"); 
            }

            (*commandPools)[QueueType::Transfer] = commandPool;
        } else if (familyIndices.computeFamily != familyIndices.transferFamily
                   && familyIndices.transferFamily == familyIndices.graphicsFamily) 
        {
            commandPoolInfo.queueFamilyIndex = familyIndices.computeFamily;

            if (vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create compute command pool!"); 
            }

            (*commandPools)[QueueType::Compute] = commandPool;
        }
    }

    void VulkanDevice::createMemoryAllocator(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator *memoryAllocator) {
        VmaAllocatorCreateInfo allocatorCreateInfo = {
            .vulkanApiVersion = VK_API_VERSION_1_3,
            .physicalDevice = physicalDevice,
            .device = device,
            .instance = instance
        };

        if (vmaCreateAllocator(&allocatorCreateInfo, memoryAllocator) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create memory allocator!");
        }
    }

    void VulkanDevice::createDescriptorPool(VkDevice device, VkDescriptorPool* descriptorPool) {
        VkDescriptorType desiredDescriptor[4] = {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
        };

        VkDescriptorPoolSize descPoolSizes[4];
        for (uint8_t i = 0; i < 4; i++) {
            VkDescriptorPoolSize descPoolSize {
                .type = desiredDescriptor[i],
                .descriptorCount = 100
            };
        }

        VkDescriptorPoolCreateInfo descPoolInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 15,
            .poolSizeCount = 4,
            .pPoolSizes = descPoolSizes
        };

        if (vkCreateDescriptorPool(device, &descPoolInfo, nullptr, descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    VulkanDevice::VulkanDevice(DeviceDescriptor desc) : desc(desc) {
        createInstance(desc, &instance, &this->debugMessenger);
        pickPhysicalDevice(instance, &physicalDevice, &this->deviceProperties);
        createLogicalDevice(instance, physicalDevice, &device, &this->queues, &this->commandPools);
        createMemoryAllocator(instance, physicalDevice, device, &this->memoryAllocator);
        createDescriptorPool(device, &this->descriptorPool);
    }

    std::shared_ptr<Device> VulkanFactory::createDevice(DeviceDescriptor desc) {
        return std::make_shared<VulkanDevice>(desc);
    }
}