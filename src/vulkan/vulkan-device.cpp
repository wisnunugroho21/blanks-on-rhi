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

    void createInstance(DeviceDescriptor desc, VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char *> extensions;

        if (desc.enableDebug) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

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

    void pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *selectedPhysicalDevice, VkPhysicalDeviceProperties *deviceProperties) {
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
            }
        }
    }

    void createLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice *device, std::vector<std::shared_ptr<Queue>>* queues) {
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        QueueFamilyIndices familyIndices = findQueueFamilies(physicalDevice);
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo graphicQueueCreateInfo{};
        graphicQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicQueueCreateInfo.queueFamilyIndex = familyIndices.graphicsFamily;
        graphicQueueCreateInfo.queueCount = 1u;
        graphicQueueCreateInfo.pQueuePriorities = &queuePriority;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {graphicQueueCreateInfo};

        if (familyIndices.computeFamily != familyIndices.transferFamily
            && familyIndices.transferFamily != familyIndices.graphicsFamily
            && familyIndices.computeFamily != familyIndices.graphicsFamily) 
        {
            VkDeviceQueueCreateInfo presentQueueCreateInfo{};
            presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            presentQueueCreateInfo.queueFamilyIndex = familyIndices.computeFamily;
            presentQueueCreateInfo.queueCount = familyIndices.computeCount;
            presentQueueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.emplace_back(presentQueueCreateInfo);

            VkDeviceQueueCreateInfo transferQueueCreateInfo{};
            transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            transferQueueCreateInfo.queueFamilyIndex = familyIndices.transferFamily;
            transferQueueCreateInfo.queueCount = familyIndices.transferCount;
            transferQueueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.emplace_back(transferQueueCreateInfo);
        } else if (familyIndices.computeFamily == familyIndices.transferFamily
                   && familyIndices.computeFamily != familyIndices.graphicsFamily) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = familyIndices.computeFamily;
            queueCreateInfo.queueCount = familyIndices.computeCount;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.emplace_back(queueCreateInfo);
        } else if (familyIndices.computeFamily != familyIndices.transferFamily
                   && familyIndices.computeFamily == familyIndices.graphicsFamily) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = familyIndices.transferFamily;
            queueCreateInfo.queueCount = familyIndices.transferCount;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.emplace_back(queueCreateInfo);
        } else if (familyIndices.computeFamily != familyIndices.transferFamily
                   && familyIndices.transferFamily == familyIndices.graphicsFamily) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = familyIndices.computeFamily;
            queueCreateInfo.queueCount = familyIndices.computeCount;
            queueCreateInfo.pQueuePriorities = &queuePriority;

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

            QueueDescriptor desc = QueueDescriptor()
                .setType(QueueType::Graphic)
                .setIndex(i);

            std::shared_ptr<Queue> a = std::make_shared<VulkanQueue>(desc, vulkanQueue);
            queues->push_back(std::make_shared<VulkanQueue>(desc, vulkanQueue));
        }

        for (uint32_t i = 0; i < familyIndices.computeCount; i++) {
            VkQueue vulkanQueue;
            vkGetDeviceQueue(*device, familyIndices.computeFamily, i, &vulkanQueue);

            QueueDescriptor desc = QueueDescriptor()
                .setType(QueueType::Compute)
                .setIndex(i);

            queues->push_back(std::make_shared<VulkanQueue>(desc, vulkanQueue));
        }

        for (uint32_t i = 0; i < familyIndices.transferCount; i++) {
            VkQueue vulkanQueue;
            vkGetDeviceQueue(*device, familyIndices.transferFamily, i, &vulkanQueue);

            QueueDescriptor desc = QueueDescriptor()
                .setType(QueueType::Transfer)
                .setIndex(i);

            queues->push_back(std::make_shared<VulkanQueue>(desc, vulkanQueue));
        }
    }

    void createMemoryAllocator(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator *memoryAllocator) {
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorCreateInfo.physicalDevice = physicalDevice;
        allocatorCreateInfo.device = device;
        allocatorCreateInfo.instance = instance;

        vmaCreateAllocator(&allocatorCreateInfo, memoryAllocator);
    }

    std::shared_ptr<Device> VulkanFactory::createDevice(DeviceDescriptor desc) {
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDeviceProperties deviceProperties;
        VmaAllocator memoryAllocator;

        std::vector<std::shared_ptr<Queue>> queues;

        createInstance(desc, &instance, &debugMessenger);
        pickPhysicalDevice(instance, &physicalDevice, &deviceProperties);
        createLogicalDevice(instance, physicalDevice, &device, &queues);
        createMemoryAllocator(instance, physicalDevice, device, &memoryAllocator);

        return std::make_shared<VulkanDevice>(desc, instance, physicalDevice, device, 
            debugMessenger, deviceProperties, memoryAllocator, queues);
    }
}