#include "vulkan-backend.hpp"

namespace RHI {
    void VulkanBeginRenderPassCommand::execute(CommandBuffer* cmd) {
        std::vector<VkImageView> imageViews;

        for (auto &&colorTextureView : this->colorTextureViews) {
            imageViews.emplace_back(static_cast<VulkanTextureView*>(colorTextureView)->getNative());
        }

        if (this->depthStencilTextureView != nullptr) {
            imageViews.emplace_back(static_cast<VulkanTextureView*>(this->depthStencilTextureView)->getNative());
        }

        VkRenderPass rp = static_cast<VulkanRenderGraph*>(this->renderGraph)->getNative()[renderPassIndex].renderPass;

        VkFramebufferCreateInfo fbInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = rp,
            .attachmentCount = static_cast<Uint32>(imageViews.size()),
            .pAttachments = imageViews.data(),
            .width = this->size.width,
            .height = this->size.height,
            .layers = this->size.depthOrArrayLayers
        };

        VulkanCommandBuffer* vulkanCmd = static_cast<VulkanCommandBuffer*>(cmd);

        VkFramebuffer fb;
        if (vkCreateFramebuffer(vulkanCmd->getDevice()->getNative(), &fbInfo, nullptr, &fb) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }

        vulkanCmd->getFrameBuffers().emplace_back(fb);

        RenderPassDescriptor desc = this->renderGraph->getDesc().renderPasses[renderPassIndex];

        std::vector<VkClearValue> clearValues;
        for (auto &&colorAttachment : desc.colorAttachments) {
            clearValues.emplace_back(VkClearValue{
                .color = VkClearColorValue{
                    .float32 = {
                        colorAttachment.clearValue.r, 
                        colorAttachment.clearValue.g, 
                        colorAttachment.clearValue.b, 
                        colorAttachment.clearValue.a
                    }
                }
            });
        }

        if (desc.depthStencilAttachment.format != TextureFormat::eUndefined) {
            clearValues.emplace_back(VkClearValue{
                .depthStencil = VkClearDepthStencilValue{
                    .depth = desc.depthStencilAttachment.depthClearValue,
                    .stencil = desc.depthStencilAttachment.stencilClearValue
                }
            });
        }

        VkRenderPassBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = rp,
            .framebuffer = fb,
            .clearValueCount = static_cast<Uint32>(clearValues.size()),
            .pClearValues = clearValues.data(),
            .renderArea = VkRect2D{
                .extent = VkExtent2D{
                    .width = this->size.width,
                    .height = this->size.height
                }
            }
        };

        vkCmdBeginRenderPass(vulkanCmd->getNative(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
}