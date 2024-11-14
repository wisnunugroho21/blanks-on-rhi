#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    std::shared_ptr<Texture> VulkanDevice::createTexture(TextureDescriptor desc) {
        VkImage image;
        VmaAllocation memoryAllocation;

        VkImageCreateInfo imageInfo;
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = convertDimensionIntoVulkan(desc.dimension);
        imageInfo.format = convertTextureFormatIntoVulkan(desc.format);
        imageInfo.extent.width = desc.size.width;
        imageInfo.extent.height = desc.size.height;
        imageInfo.extent.depth = desc.size.depth;
        imageInfo.mipLevels = desc.mipLevelCount;
        imageInfo.arrayLayers = desc.sliceLayersNum;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = convertImageUsageIntoVulkan(desc.usage);
        imageInfo.initialLayout = convertTextureStateIntoVulkan(desc.initialState);

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
        imageViewInfo.format = convertTextureFormatIntoVulkan(desc.format);
        imageViewInfo.viewType = convertTextureViewDimensionIntoVulkan(desc.dimension);
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

        return std::make_shared<VulkanTextureView>(desc, this, this->device, imageView);
    }

    VulkanTextureView::~VulkanTextureView() {
        vkDestroyImageView(this->device->getNative(), this->imageView, nullptr);
    }
}