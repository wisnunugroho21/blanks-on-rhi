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
            stage != PipelineStage::eEarlyFragmentTest && stage != PipelineStage::eLateFragmentTest && 
            access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_SHADER_READ_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eEarlyFragmentTest && stage != PipelineStage::eLateFragmentTest &&
            access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eEarlyFragmentTest && stage != PipelineStage::eLateFragmentTest && 
            access == ResourceAccess::eReadWrite) 
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

        else if ((stage == PipelineStage::eEarlyFragmentTest || stage == PipelineStage::eLateFragmentTest)
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)
            && access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        }

        else if ((stage == PipelineStage::eEarlyFragmentTest || stage == PipelineStage::eLateFragmentTest)
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)
            && access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        else if ((stage == PipelineStage::eEarlyFragmentTest || stage == PipelineStage::eLateFragmentTest)
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)
            && access == ResourceAccess::eReadWrite) 
        {
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eEarlyFragmentTest && stage != PipelineStage::eLateFragmentTest 
            && access == ResourceAccess::eReadOnly) 
        {
            return VK_ACCESS_SHADER_READ_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eEarlyFragmentTest && stage != PipelineStage::eLateFragmentTest 
            && access == ResourceAccess::eWriteOnly) 
        {
            return VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eEarlyFragmentTest && stage != PipelineStage::eLateFragmentTest 
            && access == ResourceAccess::eReadWrite) 
        {
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        }

        else if (access == ResourceAccess::eReadOnly) {
            return VK_ACCESS_MEMORY_READ_BIT;
        }

        else if (access == ResourceAccess::eWriteOnly) {
            return VK_ACCESS_MEMORY_WRITE_BIT;
        }
        
        return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    }

    TextureState findTextureState(PipelineStage stage, ResourceAccess access, Texture *texture) {
        if (stage == PipelineStage::ePresent) {
            return TextureState::ePresent;
        }
        
        if (stage == PipelineStage::eTransfer && texture->getDesc().usage & std::to_underlying(TextureUsage::eCopySrc)) {
            return TextureState::eCopySrc;
        }

        else if (stage == PipelineStage::eTransfer && texture->getDesc().usage & std::to_underlying(TextureUsage::eCopyDst)) {
            return TextureState::eCopyDst;
        }

        else if (stage == PipelineStage::eAttachmentOutput && texture->getDesc().usage & std::to_underlying(TextureUsage::eColorAttachment)) {
            return TextureState::eColorAttachment;
        }

        else if ((stage == PipelineStage::eEarlyFragmentTest || stage == PipelineStage::eLateFragmentTest) 
            && texture->getDesc().usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)) 
        {
            return TextureState::eDepthStencilAttachment;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && texture->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
        {
            return TextureState::eColorTextureBinding;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && texture->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
        {
            return TextureState::eColorTextureBinding;
        }

        else if (stage != PipelineStage::eTransfer && stage != PipelineStage::eAttachmentOutput && 
            stage != PipelineStage::eLateFragmentTest && texture->getDesc().usage & std::to_underlying(TextureUsage::eStorageBinding)) 
        {
            return TextureState::eStorageBinding;
        }

        return TextureState::eUndefined;
    }

    void VulkanBarrier::recordBufferBarrier(CommandBuffer* commandBuffer, BufferInfo target, PipelineStage stage, ResourceAccess access) {
        bool isBarrierExist = false;

        for (auto &&curBufferState : this->curBufferStates) {
            if (curBufferState.desc.buffer == target.buffer 
                && curBufferState.desc.size == target.size
                && curBufferState.desc.offset == target.offset)
            {
                VkBufferMemoryBarrier bufferBarrier{
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .srcAccessMask = convertBufferAccessIntoVulkan(curBufferState.stage, curBufferState.access, curBufferState.desc.buffer),
                    .dstAccessMask = convertBufferAccessIntoVulkan(stage, access, target.buffer),
                    .buffer = dynamic_cast<VulkanBuffer*>(target.buffer)->getNative(),
                    .offset = target.offset,
                    .size = target.size
                };

                vkCmdPipelineBarrier(
                    static_cast<VulkanCommandBuffer*>(commandBuffer)->getNative(),
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
                .desc = target,
                .stage = stage,
                .access = access
            });
        }
    }

    void VulkanBarrier::recordTextureBarrier(CommandBuffer* commandBuffer, TextureView* target, TextureState state,
        PipelineStage stage, ResourceAccess access) 
    {   
        bool isBarrierExist = false;

        for (auto &&curTextureState : this->curTextureStates) {
            if (curTextureState.target->getTexture() == target->getTexture())
            {
                // TextureState newState = findTextureState(stage, access, curTextureState.target->getTexture());

                VkImageMemoryBarrier imageBarrier{
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .srcAccessMask = convertTextureAccessIntoVulkan(curTextureState.stage, curTextureState.access, curTextureState.target->getTexture()),
                    .dstAccessMask = convertTextureAccessIntoVulkan(stage, access, target->getTexture()),
                    .oldLayout = convertTextureStateIntoVulkan(dynamic_cast<VulkanTexture*>(target->getTexture())->state),
                    .newLayout = convertTextureStateIntoVulkan(state),
                    .subresourceRange.aspectMask = convertAspectIntoVulkan(target->getDesc().subresource.aspect),
                    .subresourceRange.baseArrayLayer = target->getDesc().subresource.baseArrayLayer,
                    .subresourceRange.layerCount = target->getDesc().subresource.arrayLayerCount,
                    .subresourceRange.baseMipLevel = target->getDesc().subresource.baseMipLevel,
                    .subresourceRange.levelCount = target->getDesc().subresource.mipLevelCount
                };

                vkCmdPipelineBarrier(
                    static_cast<VulkanCommandBuffer*>(commandBuffer)->getNative(),
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

                dynamic_cast<VulkanTexture*>(target->getTexture())->state = state;
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