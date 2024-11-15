#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<RenderPassEncoder> VulkanCommandEncoder::beginRenderPass(RenderPassDescriptor desc) {
        std::vector<VkRenderingAttachmentInfo> colorRenderAttachInfos{};

        for (auto &&colorAttachment : desc.colorAttachments) {
            VkRenderingAttachmentInfo colorRenderAttachInfo{};

            colorRenderAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colorRenderAttachInfo.imageView = dynamic_cast<VulkanTextureView*>(colorAttachment.targetView)->getNative();
            colorRenderAttachInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            if (colorAttachment.resolveTargetView != nullptr) {
                colorRenderAttachInfo.resolveImageView = dynamic_cast<VulkanTextureView*>(colorAttachment.resolveTargetView)->getNative();
                colorRenderAttachInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorRenderAttachInfo.resolveMode = convertResolveModeIntoVulkan(colorAttachment.resolveMode);
            }

            colorRenderAttachInfo.loadOp = convertLoadOpIntoVulkan(colorAttachment.loadOp);
            colorRenderAttachInfo.storeOp = convertStoreOpIntoVulkan(colorAttachment.storeOp);

            colorRenderAttachInfo.clearValue.color.float32[0] = colorAttachment.clearValue.r;
            colorRenderAttachInfo.clearValue.color.float32[1] = colorAttachment.clearValue.g;
            colorRenderAttachInfo.clearValue.color.float32[2] = colorAttachment.clearValue.b;
            colorRenderAttachInfo.clearValue.color.float32[3] = colorAttachment.clearValue.a;
            
            colorRenderAttachInfos.emplace_back(colorRenderAttachInfo);
        }

        VkRenderingAttachmentInfo depthRenderAttachInfo{};

        depthRenderAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthRenderAttachInfo.imageView = dynamic_cast<VulkanTextureView*>(desc.depthAttachment.targetView)->getNative();
        depthRenderAttachInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

        depthRenderAttachInfo.loadOp = convertLoadOpIntoVulkan(desc.depthAttachment.loadOp);
        depthRenderAttachInfo.storeOp = convertStoreOpIntoVulkan(desc.depthAttachment.storeOp);

        depthRenderAttachInfo.clearValue.depthStencil.depth = desc.depthAttachment.clearValue;

        VkRenderingAttachmentInfo stencilRenderAttachInfo{};

        stencilRenderAttachInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        stencilRenderAttachInfo.imageView = dynamic_cast<VulkanTextureView*>(desc.stencilAttachment.targetView)->getNative();
        stencilRenderAttachInfo.imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;

        stencilRenderAttachInfo.loadOp = convertLoadOpIntoVulkan(desc.stencilAttachment.loadOp);
        stencilRenderAttachInfo.storeOp = convertStoreOpIntoVulkan(desc.stencilAttachment.storeOp);

        stencilRenderAttachInfo.clearValue.depthStencil.stencil = desc.stencilAttachment.clearValue;

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.extent.height = desc.depthAttachment.targetView->getTexture()->getDesc().size.height;
        renderingInfo.renderArea.extent.width = desc.depthAttachment.targetView->getTexture()->getDesc().size.width;
        renderingInfo.renderArea.offset.x = 0;
        renderingInfo.renderArea.offset.y = 0;
        renderingInfo.layerCount = desc.depthAttachment.targetView->getTexture()->getDesc().sliceLayersNum;
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorRenderAttachInfos.size());
        renderingInfo.pColorAttachments = colorRenderAttachInfos.data();
        renderingInfo.pDepthAttachment = &depthRenderAttachInfo;
        renderingInfo.pStencilAttachment = &stencilRenderAttachInfo;
        
        vkCmdBeginRendering(this->commandBuffer, &renderingInfo);

        return std::make_shared<VulkanRenderPassEncoder>(desc, this);
    }
    
    void VulkanRenderPassEncoder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
        VkViewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;

        vkCmdSetViewport(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0,
            1,
            &viewport
        );
    }

    void VulkanRenderPassEncoder::setViewport(Viewport viewport) {
        this->setViewport(viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth);
    }

    void VulkanRenderPassEncoder::setViewport(std::vector<Viewport> viewports) {
        std::vector<VkViewport> vulkanViewports{};

        for (auto &&viewport : viewports) {
            vulkanViewports.emplace_back(convertViewportIntoVulkan(viewport));
        }

        vkCmdSetViewport(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0,
            static_cast<uint32_t>(vulkanViewports.size()),
            vulkanViewports.data()
        );
    }

    void VulkanRenderPassEncoder::setScissorRect(Uint32 x, Uint32 y, Uint32 width, Uint32 height) {
        VkRect2D rect2d{};
        rect2d.extent.width = width;
        rect2d.extent.height = height;
        rect2d.offset.x = x;
        rect2d.offset.y = y;

        vkCmdSetScissor(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0,
            1,
            &rect2d
        );
    }

    void VulkanRenderPassEncoder::setScissorRect(Rect2D scissor) {
        this->setScissorRect(scissor.x, scissor.y, scissor.width, scissor.height);
    }

    void VulkanRenderPassEncoder::setScissorRect(std::vector<Rect2D> scissors) {
        std::vector<VkRect2D> vulkanScissors{};

        for (auto &&scissor : scissors) {
            vulkanScissors.emplace_back(convertRect2DIntoVulkan(scissor));
        }

        vkCmdSetScissor(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0,
            static_cast<uint32_t>(vulkanScissors.size()),
            vulkanScissors.data()
        );
    }

    void VulkanRenderPassEncoder::setLineWidth(float lineWidth) {
        vkCmdSetLineWidth(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            lineWidth
        );
    }

    void VulkanRenderPassEncoder::setDepthBias(float constant, float clamp, float slopeScale) {
        vkCmdSetDepthBias(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            constant,
            clamp,
            slopeScale
        );
    }

    void VulkanRenderPassEncoder::setDepthBias(DepthBias depthBias) {
        this->setDepthBias(depthBias.constant, depthBias.clamp, depthBias.slopeScale);
    }

    void VulkanRenderPassEncoder::setBlendConstant(float r, float g, float b, float a) {
        float blendConstants[4];
        blendConstants[0] = r;
        blendConstants[1] = g;
        blendConstants[2] = b;
        blendConstants[3] = a;

        vkCmdSetBlendConstants(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            blendConstants
        );
    }

    void VulkanRenderPassEncoder::setBlendConstant(Color color) {
        this->setBlendConstant(color.r, color.g, color.b, color.a);
    }

    void VulkanRenderPassEncoder::setDepthBounds(float min, float max) {
        vkCmdSetDepthBounds(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            min,
            max
        );
    }

    void VulkanRenderPassEncoder::setStencilCompareMask(Uint32 compareMask) {
        vkCmdSetStencilCompareMask(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_STENCIL_FACE_FRONT_AND_BACK,
            compareMask
        );
    }

    void VulkanRenderPassEncoder::setStencilWriteMask(Uint32 writeMask) {
        vkCmdSetStencilCompareMask(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_STENCIL_FACE_FRONT_AND_BACK,
            writeMask
        );
    }

    void VulkanRenderPassEncoder::setStencilReference(Uint32 reference) {
        vkCmdSetStencilReference(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_STENCIL_FACE_FRONT_AND_BACK,
            reference
        );
    }

    void VulkanRenderPassEncoder::end() {
        vkCmdEndRendering(dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative());
    }
}