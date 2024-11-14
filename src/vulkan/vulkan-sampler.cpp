#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<Sampler> VulkanDevice::createSampler(SamplerDescriptor desc) {
        VkSampler sampler;

        VkSamplerCreateInfo samplerInfo;
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = convertFilterToVulkan(desc.magFilter);
        samplerInfo.minFilter = convertFilterToVulkan(desc.minFilter);
        samplerInfo.mipmapMode = convertMipmapFilterToVulkan(desc.mipmapFilter);
        samplerInfo.addressModeU = convertAddressModeToVulkan(desc.addressModeU);
        samplerInfo.addressModeV = convertAddressModeToVulkan(desc.addressModeV);
        samplerInfo.addressModeW = convertAddressModeToVulkan(desc.addressModeW);
        samplerInfo.compareEnable = desc.compare != CompareFunction::eNever ? VK_TRUE : VK_FALSE;
        samplerInfo.compareOp = convertCompareOpToVulkan(desc.compare);
        samplerInfo.anisotropyEnable = desc.maxAnisotropy > 1 ? VK_TRUE : VK_FALSE;
        samplerInfo.maxAnisotropy = desc.maxAnisotropy;
        samplerInfo.minLod = desc.lodMinClamp;
        samplerInfo.maxLod = desc.lodMaxClamp;
        samplerInfo.borderColor = convertBorderColorToVulkan(desc.borderColor);

        if (vkCreateSampler(this->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sampler!");
        }

        return std::make_shared<VulkanSampler>(desc, this, sampler);
    }

    VulkanSampler::~VulkanSampler() {
        vkDestroySampler(this->device->getNative(), this->sampler, nullptr);
    }
}