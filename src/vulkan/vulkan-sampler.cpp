#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<Sampler> VulkanDevice::createSampler(SamplerDescriptor desc) {
        VkSamplerCreateInfo samplerInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = convertFilterToVulkan(desc.magFilter),
            .minFilter = convertFilterToVulkan(desc.minFilter),
            .mipmapMode = convertMipmapFilterToVulkan(desc.mipmapFilter),
            
            .addressModeU = convertAddressModeToVulkan(desc.addressModeU),
            .addressModeV = convertAddressModeToVulkan(desc.addressModeV),
            .addressModeW = convertAddressModeToVulkan(desc.addressModeW),
            
            .compareEnable = desc.compare != CompareFunction::eNever ? VK_TRUE : VK_FALSE,
            .compareOp = convertCompareOpToVulkan(desc.compare),

            .anisotropyEnable = desc.maxAnisotropy > 1 ? VK_TRUE : VK_FALSE,
            .maxAnisotropy = desc.maxAnisotropy,

            .minLod = desc.lodMinClamp,
            .maxLod = desc.lodMaxClamp,
            
            .borderColor = convertBorderColorToVulkan(desc.borderColor)
        };

        VkSampler sampler;
        if (vkCreateSampler(this->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }

        return std::make_shared<VulkanSampler>(desc, this, sampler);
    }

    VulkanSampler::~VulkanSampler() {
        vkDestroySampler(this->device->getNative(), this->sampler, nullptr);
    }
}