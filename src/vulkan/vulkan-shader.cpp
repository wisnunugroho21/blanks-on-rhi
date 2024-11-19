#include "vulkan-backend.hpp"

#include <fstream>

namespace RHI {
    std::vector<char> readFile(const std::string &filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader file!");
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize >(fileSize));

        file.close();
        return buffer;
    }

    std::shared_ptr<ShaderModule> VulkanDevice::createShaderModule(ShaderModuleDescriptor desc) {
        std::vector<char> code = readFile(desc.filename);

        VkShaderModuleCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t *>(code.data())
        };

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(this->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }

        return std::make_shared<VulkanShaderModule>(desc, this, shaderModule);
    }

    VulkanShaderModule::~VulkanShaderModule() {
        vkDestroyShaderModule(this->device->getNative(), this->shaderModule, nullptr);
    }
}