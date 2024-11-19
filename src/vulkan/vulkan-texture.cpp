#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    std::shared_ptr<Texture> VulkanDevice::createTexture(TextureDescriptor desc) {
        VkImageCreateInfo imageInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = convertDimensionIntoVulkan(desc.dimension),
            .format = convertTextureFormatIntoVulkan(desc.format),
            .extent.width = desc.size.width,
            .extent.height = desc.size.height,
            .extent.depth = desc.size.depth,
            .mipLevels = desc.mipLevelCount,
            .arrayLayers = desc.sliceLayersNum,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = convertImageUsageIntoVulkan(desc.usage),
            .initialLayout = convertTextureStateIntoVulkan(desc.initialState)
        };

        VmaAllocationCreateInfo allocInfo{
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        if (desc.dimension == TextureDimension::e2D && desc.sliceLayersNum == 6) {
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }

        VkImage image;
        VmaAllocation memoryAllocation;

        if (vmaCreateImage(this->memoryAllocator, &imageInfo, &allocInfo, &image, &memoryAllocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture!");
        }

        return std::make_shared<VulkanTexture>(desc, this, image, memoryAllocation);
    }

    VulkanTexture::~VulkanTexture() {
        vmaDestroyImage(this->device->getMemoryAllocator(), this->image, this->memoryAllocation);
    }

    std::shared_ptr<TextureView> VulkanTexture::createView(TextureViewDescriptor desc) {
        VkImageView imageView;

        VkImageViewCreateInfo imageViewInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = this->image,
            .format = convertTextureFormatIntoVulkan(desc.format),
            .viewType = convertTextureViewDimensionIntoVulkan(desc.dimension),
            .subresourceRange.aspectMask = convertAspectIntoVulkan(desc.subresource.aspect),
            .subresourceRange.baseMipLevel = desc.subresource.baseMipLevel,
            .subresourceRange.levelCount = desc.subresource.mipLevelCount,
            .subresourceRange.baseArrayLayer = desc.subresource.baseArrayLayer,
            .subresourceRange.layerCount = desc.subresource.arrayLayerCount,
            .components.r = VK_COMPONENT_SWIZZLE_R,
            .components.g = VK_COMPONENT_SWIZZLE_G,
            .components.b = VK_COMPONENT_SWIZZLE_B,
            .components.a = VK_COMPONENT_SWIZZLE_A
        };

        if (vkCreateImageView(this->device->getNative(), &imageViewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture view!");
        }

        return std::make_shared<VulkanTextureView>(desc, this, this->device, imageView);
    }

    VulkanTextureView::~VulkanTextureView() {
        vkDestroyImageView(this->device->getNative(), this->imageView, nullptr);
    }
}