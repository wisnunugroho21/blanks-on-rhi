#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    VkAccessFlags convertBufferAccessIntoVulkan(PipelineStage stage, ResourceAccess access, Buffer *buffer) {
        if (stage == PipelineStage::eTransfer && access == ResourceAccess::eReadOnly) {
            return VK_ACCESS_TRANSFER_READ_BIT;
        } 
        
        else if (stage == PipelineStage::eTransfer && access == ResourceAccess::eWriteOnly) {
            return VK_ACCESS_TRANSFER_WRITE_BIT;
        } 
        
        else if (stage == PipelineStage::eTransfer && access == ResourceAccess::eReadWrite) {
            return VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        } 
        
        else if (stage == PipelineStage::eVertex && buffer->getDesc().usage & std::to_underlying(BufferUsage::eVertex)) {
            return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        } 
        
        else if (stage == PipelineStage::eVertex && buffer->getDesc().usage & std::to_underlying(BufferUsage::eIndex)) {
            return VK_ACCESS_INDEX_READ_BIT;
        }

        else if (stage == PipelineStage::eVertex && buffer->getDesc().usage & std::to_underlying(BufferUsage::eIndirect)) {
            return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        } 
        
        else if (buffer->getDesc().usage & std::to_underlying(BufferUsage::eUniform)) {
            return VK_ACCESS_UNIFORM_READ_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_SHADER_READ_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && access == ResourceAccess::eReadWrite) 
        {
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_MEMORY_READ_BIT;
        }

        else if (access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_MEMORY_WRITE_BIT;
        }
        
        return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    }

    VkAccessFlags convertTextureAccessIntoVulkan(PipelineStage stage, ResourceAccess access, Texture *texture) {
        if (stage == PipelineStage::eTransfer && access == ResourceAccess::eReadOnly) {
            return VK_ACCESS_TRANSFER_READ_BIT;
        } 
        
        else if (stage == PipelineStage::eTransfer && access == ResourceAccess::eWriteOnly) {
            return VK_ACCESS_TRANSFER_WRITE_BIT;
        } 
        
        else if (stage == PipelineStage::eTransfer && access == ResourceAccess::eReadWrite) {
            return VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }

        else if (stage == PipelineStage::eAttachmentOutput 
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eColorAttachment)
            && access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        }

        else if (stage == PipelineStage::eAttachmentOutput 
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eColorAttachment)
            && access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        else if (stage == PipelineStage::eAttachmentOutput 
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eColorAttachment)
            && access == ResourceAccess::eReadWrite) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        else if (stage == PipelineStage::eLateFragmentTest
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)
            && access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        }

        else if (stage == PipelineStage::eLateFragmentTest
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)
            && access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        else if (stage == PipelineStage::eLateFragmentTest
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)
            && access == ResourceAccess::eReadWrite) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_SHADER_READ_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && access == ResourceAccess::eReadWrite) 
        {
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_MEMORY_READ_BIT;
        }

        else if (access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_MEMORY_WRITE_BIT;
        }
        
        return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    }

    VkImageLayout findImageLayout(PipelineStage stage, ResourceAccess access, Texture *texture) {
        if (stage == PipelineStage::eTransfer && texture->getDesc().usage & std::to_underlying(TextureUsage::eCopySrc)) {
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        }

        else if (stage == PipelineStage::eTransfer && texture->getDesc().usage & std::to_underlying(TextureUsage::eCopyDst)) {
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        }

        else if (stage == PipelineStage::eAttachmentOutput && texture->getDesc().usage & std::to_underlying(TextureUsage::eColorAttachment)) {
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        else if (stage == PipelineStage::eLateFragmentTest && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)) {
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && texture->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
        {
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && texture->getDesc().usage & std::to_underlying(TextureUsage::eStorageBinding)) 
        {
            return VK_IMAGE_LAYOUT_GENERAL;
        }

        return VK_IMAGE_LAYOUT_GENERAL;
    }

    std::shared_ptr<CommandEncoder> VulkanDevice::beginCommandEncoder(CommandEncoderDescriptor desc) {
        VkCommandBufferAllocateInfo commandBufferInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = this->commandPools[desc.queueType],
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer commandBuffer;
        if (vkAllocateCommandBuffers(this->device, &commandBufferInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }

        return std::make_shared<VulkanCommandEncoder>(desc, this, commandBuffer);
    }

    void VulkanBarrier::recordBufferBarrier(VkCommandBuffer commandBuffer, 
        PipelineStage stage, ResourceAccess access, BufferInfo desc) 
    {
        bool isBarrierExist = false;

        for (auto &&curBufferState : this->curBufferStates) {
            if (curBufferState.stage == stage && curBufferState.access == access 
                && curBufferState.desc.buffer == desc.buffer 
                && curBufferState.desc.size == desc.size
                && curBufferState.desc.offset == desc.offset)
            {
                VkBufferMemoryBarrier bufferBarrier{
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .srcAccessMask = convertBufferAccessIntoVulkan(curBufferState.stage, curBufferState.access, curBufferState.desc.buffer),
                    .dstAccessMask = convertBufferAccessIntoVulkan(stage, access, desc.buffer),
                    .buffer = dynamic_cast<VulkanBuffer*>(desc.buffer)->getNative(),
                    .offset = desc.offset,
                    .size = desc.size
                };

                vkCmdPipelineBarrier(
                    commandBuffer,
                    convertPipelineStageIntoVulkan(curBufferState.stage),
                    convertPipelineStageIntoVulkan(stage),
                    0,
                    0,
                    nullptr,
                    1,
                    &bufferBarrier,
                    0,
                    nullptr
                );

                curBufferState.stage = stage;
                curBufferState.access = access;

                isBarrierExist = true;
                break;
            }
        }

        if (!isBarrierExist) {
            this->curBufferStates.emplace_back(BufferBarrierState {
                .desc = desc,
                .stage = stage,
                .access = access
            });
        }
    }

    void VulkanBarrier::recordTextureBarrier(VkCommandBuffer commandBuffer, PipelineStage stage, 
        ResourceAccess access, TextureView* target) 
    {
        bool isBarrierExist = false;

        for (auto &&curTextureState : this->curTextureStates) {
            if (curTextureState.stage == stage && curTextureState.access == access 
                && curTextureState.target->getTexture() == target->getTexture())
            {
                VkImageLayout newLayout = findImageLayout(stage, access, curTextureState.target->getTexture());

                VkImageMemoryBarrier imageBarrier{
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .srcAccessMask = convertTextureAccessIntoVulkan(curTextureState.stage, curTextureState.access, curTextureState.target->getTexture()),
                    .dstAccessMask = convertTextureAccessIntoVulkan(stage, access, target->getTexture()),
                    .oldLayout = dynamic_cast<VulkanTexture*>(target->getTexture())->layout,
                    .newLayout = newLayout,
                    .subresourceRange.aspectMask = convertAspectIntoVulkan(target->getDesc().subresource.aspect),
                    .subresourceRange.baseArrayLayer = target->getDesc().subresource.baseArrayLayer,
                    .subresourceRange.layerCount = target->getDesc().subresource.arrayLayerCount,
                    .subresourceRange.baseMipLevel = target->getDesc().subresource.baseMipLevel,
                    .subresourceRange.levelCount = target->getDesc().subresource.mipLevelCount
                };

                vkCmdPipelineBarrier(
                    commandBuffer,
                    convertPipelineStageIntoVulkan(curTextureState.stage),
                    convertPipelineStageIntoVulkan(stage),
                    0,
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &imageBarrier
                );

                dynamic_cast<VulkanTexture*>(target->getTexture())->layout = newLayout;
                curTextureState.access = access;
                curTextureState.stage = stage;

                isBarrierExist = true;
                break;
            }
        }

        if (!isBarrierExist) {
            this->curTextureStates.emplace_back(TextureBarrierState {
                .target = target,
                .stage = stage,
                .access = access
            });
        };
    }
}