#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    VkFormat convertFormatIntoVulkan(TextureFormat format) {
        switch (format) {
            // 8-bit formats
            case TextureFormat::eR8Unorm:
                return VK_FORMAT_R8_UNORM;
            case TextureFormat::eR8Snorm:
                return VK_FORMAT_R8_SNORM;
            case TextureFormat::eR8Uint:
                return VK_FORMAT_R8_UINT;
            case TextureFormat::eR8Sint:
                return VK_FORMAT_R8_SINT;

            // 16-bit formats
            case TextureFormat::eR16Uint:
                return VK_FORMAT_R16_UINT;
            case TextureFormat::eR16Sint:
                return VK_FORMAT_R16_SINT;
            case TextureFormat::eR16Float:
                return VK_FORMAT_R16_SFLOAT;
            case TextureFormat::eRG8Unorm:
                return VK_FORMAT_R8G8_UNORM;
            case TextureFormat::eRG8Snorm:
                return VK_FORMAT_R8G8_SNORM;
            case TextureFormat::eRG8Uint:
                return VK_FORMAT_R8G8_UINT;
            case TextureFormat::eRG8Sint:
                return VK_FORMAT_R8G8_SINT;

            // 32-bit formats
            case TextureFormat::eR32Uint:
                return VK_FORMAT_R32_UINT;
            case TextureFormat::eR32Sint:
                return VK_FORMAT_R32_SINT;
            case TextureFormat::eR32Float:
                return VK_FORMAT_R32_SFLOAT;
            case TextureFormat::eRG16Uint:
                return VK_FORMAT_R16G16_UINT;
            case TextureFormat::eRG16Sint:
                return VK_FORMAT_R16G16_SINT;
            case TextureFormat::eRG16Float:
                return VK_FORMAT_R16G16_SFLOAT;
            case TextureFormat::eRGBA8Unorm:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::eRGBA8UnormSrgb:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::eRGBA8Snorm:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case TextureFormat::eRGBA8Uint:
                return VK_FORMAT_R8G8B8A8_UINT;
            case TextureFormat::eRGBA8Sint:
                return VK_FORMAT_R8G8B8A8_SINT;
            case TextureFormat::eBGRA8Unorm:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::eBGRA8UnormSrgb:
                return VK_FORMAT_B8G8R8A8_SRGB;

            // Packed 32-bit formats
            case TextureFormat::eRGB9E5Ufloat:
                return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
            case TextureFormat::eRGB10A2Uint:
                return VK_FORMAT_A2R10G10B10_UINT_PACK32;
            case TextureFormat::eRGB10A2Unorm:
                return VK_FORMAT_A2R10G10B10_UNORM_PACK32;

            // 64-bit formats
            case TextureFormat::eRG32Uint:
                return VK_FORMAT_R32G32_UINT;
            case TextureFormat::eRG32Sint:
                return VK_FORMAT_R32G32_SINT;
            case TextureFormat::eRG32Float:
                return VK_FORMAT_R32G32_SFLOAT;
            case TextureFormat::eRGBA16Uint:
                return VK_FORMAT_R16G16B16A16_UINT;
            case TextureFormat::eRGBA16Sint:
                return VK_FORMAT_R16G16B16A16_SINT;
            case TextureFormat::eRGBA16Float:
                return VK_FORMAT_R16G16B16A16_SFLOAT;

            // 128-bit formats
            case TextureFormat::eRGBA32Uint:
                return VK_FORMAT_R32G32B32A32_UINT;
            case TextureFormat::eRGBA32Sint:
                return VK_FORMAT_R32G32B32A32_SINT;
            case TextureFormat::eRGBA32Float:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
                
            // Depth/stencil formats
            case TextureFormat::eS8Uint:
                return VK_FORMAT_S8_UINT;
            case TextureFormat::eD16Unorm:
                return VK_FORMAT_D16_UNORM;
            case TextureFormat::eD24Plus:
                return VK_FORMAT_D32_SFLOAT;
            case TextureFormat::eD24PlusS8Uint:
                return VK_FORMAT_D24_UNORM_S8_UINT;
            case TextureFormat::eD32Sfloat:
                return VK_FORMAT_D32_SFLOAT;
            
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    VkImageType convertDimensionIntoVulkan(TextureDimension dimension) {
        switch (dimension) {
            case TextureDimension::e1D : return VK_IMAGE_TYPE_1D;
            case TextureDimension::e2D : return VK_IMAGE_TYPE_2D;
            case TextureDimension::e3D : return VK_IMAGE_TYPE_3D;

            default: return VK_IMAGE_TYPE_2D;
        }
    }

    VkSampleCountFlagBits convertSampleIntoVulkan(Uint32 sampleCount) {
        switch (sampleCount) {
            case 1: return VK_SAMPLE_COUNT_1_BIT;
            case 2: return VK_SAMPLE_COUNT_2_BIT;
            case 4: return VK_SAMPLE_COUNT_4_BIT;
            case 8: return VK_SAMPLE_COUNT_8_BIT;
            case 16: return VK_SAMPLE_COUNT_16_BIT;
            case 32: return VK_SAMPLE_COUNT_32_BIT;
            case 64: return VK_SAMPLE_COUNT_64_BIT;
            
            default: return VK_SAMPLE_COUNT_1_BIT;
        }
    }

    VkImageUsageFlags convertImageUsageIntoVulkan(TextureUsageFlags usage) {
        VkImageUsageFlags vulkanImageFlags = 0;

        if (usage & std::to_underlying(TextureUsage::eCopySrc)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eCopyDst)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eTextureBinding)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eStorageBinding)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eColorAttachment)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        return vulkanImageFlags;
    }

    VkImageLayout convertStateIntoVulkan(TextureState state) {
        switch (state) {
            case TextureState::eUndefined : return VK_IMAGE_LAYOUT_UNDEFINED;
            case TextureState::eStorageBinding : return VK_IMAGE_LAYOUT_GENERAL;
            case TextureState::eColorAttachment : return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case TextureState::eDepthStencilAttachment : return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            case TextureState::eColorTextureBinding : return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case TextureState::eDepthStencilTextureBinding : return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            case TextureState::eCopySrc : return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case TextureState::eCopyDst : return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case TextureState::ePresent : return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            default : return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    VkImageViewType convertViewTypeIntoVulkan(TextureViewDimension viewDimension) {
        switch (viewDimension) {
            case TextureViewDimension::e1D : return VK_IMAGE_VIEW_TYPE_1D;
            case TextureViewDimension::e2D : return VK_IMAGE_VIEW_TYPE_2D;
            case TextureViewDimension::e2DArray : return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case TextureViewDimension::eCube : return VK_IMAGE_VIEW_TYPE_CUBE;
            case TextureViewDimension::eCubeArray : return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            case TextureViewDimension::e3D : return VK_IMAGE_VIEW_TYPE_3D;
        
            default: return VK_IMAGE_VIEW_TYPE_2D;
        }
    }

    VkImageAspectFlags convertAspectIntoVulkan(TextureAspect aspect) {
        switch (aspect)
        {
            case TextureAspect::eColor : return VK_IMAGE_ASPECT_COLOR_BIT;
            case TextureAspect::eDepth : return VK_IMAGE_ASPECT_DEPTH_BIT;
            case TextureAspect::eStencil : return VK_IMAGE_ASPECT_STENCIL_BIT;
            case TextureAspect::eDepthStencil : return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            
            default: return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    std::shared_ptr<Texture> VulkanDevice::createTexture(TextureDescriptor desc) {
        VkImage image;
        VmaAllocation memoryAllocation;

        VkImageCreateInfo imageInfo;
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = convertDimensionIntoVulkan(desc.dimension);
        imageInfo.format = convertFormatIntoVulkan(desc.format);
        imageInfo.extent.width = desc.size.width;
        imageInfo.extent.height = desc.size.height;
        imageInfo.extent.depth = desc.size.depth;
        imageInfo.mipLevels = desc.mipLevelCount;
        imageInfo.arrayLayers = desc.sliceLayersNum;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = convertImageUsageIntoVulkan(desc.usage);
        imageInfo.initialLayout = convertStateIntoVulkan(desc.initialState);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if (desc.dimension == TextureDimension::e2D && desc.sliceLayersNum == 6) {
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }

        if (vmaCreateImage(this->memoryAllocator, &imageInfo, &allocInfo, &image, 
                           &memoryAllocation, nullptr) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create texture!");
        }

        return std::make_shared<VulkanTexture>(desc, this, image, memoryAllocation);
    }

    VulkanTexture::~VulkanTexture() {
        vmaDestroyImage(this->device->getMemoryAllocator(), this->image, this->memoryAllocation);
    }

    std::shared_ptr<TextureView> VulkanTexture::createView(TextureViewDescriptor desc) {
        VkImageView imageView;

        VkImageViewCreateInfo imageViewInfo;
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = this->image;
        imageViewInfo.format = convertFormatIntoVulkan(desc.format);
        imageViewInfo.viewType = convertViewTypeIntoVulkan(desc.dimension);
        imageViewInfo.subresourceRange.aspectMask = convertAspectIntoVulkan(desc.subresource.aspect);
        imageViewInfo.subresourceRange.baseMipLevel = desc.subresource.baseMipLevel;
        imageViewInfo.subresourceRange.levelCount = desc.subresource.mipLevelCount;
        imageViewInfo.subresourceRange.baseArrayLayer = desc.subresource.baseArrayLayer;
        imageViewInfo.subresourceRange.layerCount = desc.subresource.arrayLayerCount;
        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

        if (vkCreateImageView(this->device->getNative(), &imageViewInfo, 
                              nullptr, &imageView) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create texture view!");
        }

        return std::make_shared<VulkanTextureView>(desc, this->device, imageView);
    }

    VulkanTextureView::~VulkanTextureView() {
        vkDestroyImageView(this->device->getNative(), this->imageView, nullptr);
    }
}