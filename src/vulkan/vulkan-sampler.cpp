#include "vulkan-backend.hpp"

namespace RHI {
    VkSamplerAddressMode convertAddressModeToVulkan(AddressMode addressMode) {
        switch (addressMode) {
            case AddressMode::eRepeat : return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case AddressMode::eMirrorRepeat : return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case AddressMode::eClampToEdge : return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case AddressMode::eClampToBorder : return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            
            default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    VkFilter convertFilterToVulkan(FilterMode filterMode) {
        switch (filterMode)
        {
            case FilterMode::eNearest : return VK_FILTER_NEAREST;
            case FilterMode::eLinear : return VK_FILTER_LINEAR;
            
            default: return VK_FILTER_NEAREST;
        }
    }

    VkSamplerMipmapMode convertMipmapFilterToVulkan(MipmapFilterMode mipmapFilterMode) {
        switch (mipmapFilterMode) {
            case MipmapFilterMode::eNearest : return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case MipmapFilterMode::eLinear : return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            
            default: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }
    }

    VkCompareOp convertCompareToVulkan(CompareFunction compareFunc) {
        switch (compareFunc) {
            case CompareFunction::eNever : return VK_COMPARE_OP_NEVER;
            case CompareFunction::eEqual : return VK_COMPARE_OP_EQUAL;
            case CompareFunction::eLess : return VK_COMPARE_OP_LESS;
            case CompareFunction::eLessEqual : return VK_COMPARE_OP_LESS_OR_EQUAL;
            case CompareFunction::eGreater : return VK_COMPARE_OP_GREATER;
            case CompareFunction::eGreaterEqual : return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case CompareFunction::eNotEqual : return VK_COMPARE_OP_NOT_EQUAL;
            case CompareFunction::eAlways : return VK_COMPARE_OP_ALWAYS;
            
            default: return VK_COMPARE_OP_NEVER;
        }
    }

    VkBorderColor convertBorderColorToVulkan(BorderColor borderColor) {
        switch (borderColor)
        {
            case BorderColor::eFloatTransparentBlack : return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            case BorderColor::eIntTransparentBlack : return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            case BorderColor::eFloatOpaqueBlack : return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            case BorderColor::eIntOpaqueBlack : return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            case BorderColor::eFloatOpaqueWhite : return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            case BorderColor::eIntOpaqueWhite : return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
            
            default: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        }
    }

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
        samplerInfo.compareOp = convertCompareToVulkan(desc.compare);
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