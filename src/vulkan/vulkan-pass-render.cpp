#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<RenderPassEncoder> VulkanCommandEncoder::beginRenderPass(RenderPassDescriptor desc) {
        std::vector<VkRenderingAttachmentInfo> colorRenderAttachInfos{};

        for (auto &&colorAttachment : desc.colorAttachments) {
            VkRenderingAttachmentInfo colorRenderAttachInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = dynamic_cast<VulkanTextureView*>(colorAttachment.targetView)->getNative(),
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

                .loadOp = convertLoadOpIntoVulkan(colorAttachment.loadOp),
                .storeOp = convertStoreOpIntoVulkan(colorAttachment.storeOp),

                .clearValue.color.float32[0] = colorAttachment.clearValue.r,
                .clearValue.color.float32[1] = colorAttachment.clearValue.g,
                .clearValue.color.float32[2] = colorAttachment.clearValue.b,
                .clearValue.color.float32[3] = colorAttachment.clearValue.a
            };

            if (colorAttachment.resolveTargetView != nullptr) {
                colorRenderAttachInfo.resolveImageView = dynamic_cast<VulkanTextureView*>(colorAttachment.resolveTargetView)->getNative();
                colorRenderAttachInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorRenderAttachInfo.resolveMode = convertResolveModeIntoVulkan(colorAttachment.resolveMode);
            }
            
            colorRenderAttachInfos.emplace_back(colorRenderAttachInfo);
        }

        VkRenderingAttachmentInfo depthRenderAttachInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = dynamic_cast<VulkanTextureView*>(desc.depthAttachment.targetView)->getNative(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,

            .loadOp = convertLoadOpIntoVulkan(desc.depthAttachment.loadOp),
            .storeOp = convertStoreOpIntoVulkan(desc.depthAttachment.storeOp),

            .clearValue.depthStencil.depth = desc.depthAttachment.clearValue
        };

        VkRenderingAttachmentInfo stencilRenderAttachInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = dynamic_cast<VulkanTextureView*>(desc.stencilAttachment.targetView)->getNative(),
            .imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,

            .loadOp = convertLoadOpIntoVulkan(desc.stencilAttachment.loadOp),
            .storeOp = convertStoreOpIntoVulkan(desc.stencilAttachment.storeOp),

            .clearValue.depthStencil.stencil = desc.stencilAttachment.clearValue
        };

        VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea.extent.height = desc.depthAttachment.targetView->getTexture()->getDesc().size.height,
            .renderArea.extent.width = desc.depthAttachment.targetView->getTexture()->getDesc().size.width,

            .renderArea.offset.x = 0,
            .renderArea.offset.y = 0,
            .layerCount = desc.depthAttachment.targetView->getTexture()->getDesc().sliceLayersNum,

            .colorAttachmentCount = static_cast<uint32_t>(colorRenderAttachInfos.size()),
            .pColorAttachments = colorRenderAttachInfos.data(),

            .pDepthAttachment = &depthRenderAttachInfo,
            .pStencilAttachment = &stencilRenderAttachInfo
        };
        
        vkCmdBeginRendering(this->commandBuffer, &renderingInfo);

        return std::make_shared<VulkanRenderPassEncoder>(desc, this);
    }
    
    void VulkanRenderPassEncoder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) {
        VkViewport viewport{
            .x = x,
            .y = y,
            .width = width,
            .height = height,
            .minDepth = minDepth,
            .maxDepth = maxDepth
        };

        vkCmdSetViewport(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0,
            1,
            &viewport
        );

        this->currentState.viewports.clear();
        this->currentState.viewports.emplace_back(
            Viewport {
                .x = x,
                .y = y,
                .width = width,
                .height = height,
                .minDepth = minDepth,
                .maxDepth = maxDepth
            }
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

        this->currentState.viewports = viewports;
    }

    void VulkanRenderPassEncoder::setScissorRect(Int32 x, Int32 y, Uint32 width, Uint32 height) {
        VkRect2D rect2d{
            .extent.width = width,
            .extent.height = height,
            .offset.x = x,
            .offset.y = y
        };

        vkCmdSetScissor(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0,
            1,
            &rect2d
        );

        this->currentState.scissors.clear();
        this->currentState.scissors.emplace_back(
            Rect2D {
                .width = width,
                .height = height,
                .x = x,
                .y = y
            }
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

        this->currentState.scissors = scissors;
    }

    void VulkanRenderPassEncoder::setLineWidth(float lineWidth) {
        vkCmdSetLineWidth(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            lineWidth
        );

        this->currentState.lineWidth = lineWidth;
    }

    void VulkanRenderPassEncoder::setDepthBias(float constant, float clamp, float slopeScale) {
        vkCmdSetDepthBias(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            constant,
            clamp,
            slopeScale
        );

        this->currentState.depthBias = 
            DepthBias {
                .constant = constant,
                .clamp = clamp,
                .slopeScale = slopeScale
            };
    }

    void VulkanRenderPassEncoder::setDepthBias(DepthBias depthBias) {
        this->setDepthBias(depthBias.constant, depthBias.clamp, depthBias.slopeScale);
        
    }

    void VulkanRenderPassEncoder::setBlendConstant(float r, float g, float b, float a) {
        float blendConstants[4] { r, g, b, a };

        vkCmdSetBlendConstants(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            blendConstants
        );

        this->currentState.blendConstant = 
            Color {
                r = r,
                g = g,
                b = b,
                a = a
            };
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

        this->currentState.depthBoundMin = min;
        this->currentState.depthBoundMax = max;
    }

    void VulkanRenderPassEncoder::setStencilCompareMask(Uint32 compareMask) {
        vkCmdSetStencilCompareMask(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_STENCIL_FACE_FRONT_AND_BACK,
            compareMask
        );

        this->currentState.stencilCompareMask = compareMask;
    }

    void VulkanRenderPassEncoder::setStencilWriteMask(Uint32 writeMask) {
        vkCmdSetStencilWriteMask(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_STENCIL_FACE_FRONT_AND_BACK,
            writeMask
        );

        this->currentState.stencilWriteMask = writeMask;
    }

    void VulkanRenderPassEncoder::setStencilReference(Uint32 reference) {
        vkCmdSetStencilReference(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_STENCIL_FACE_FRONT_AND_BACK,
            reference
        );

        this->currentState.stencilReference = reference;
    }

    void VulkanRenderPassEncoder::end() {
        vkCmdEndRendering(dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative());
    }

    void VulkanRenderPassEncoder::setPipeline(RenderPipeline* pipeline) {
        vkCmdBindPipeline(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            dynamic_cast<VulkanRenderPipeline*>(pipeline)->getNative()
        );

        this->currentState.pipeline = pipeline;
    }

    void VulkanRenderPassEncoder::setBindGroup(Uint32 index, BindGroup* bindGroup, std::vector<Uint32> dynamicOffsets) {
        VulkanRenderPipeline* pipeline = dynamic_cast<VulkanRenderPipeline*>(this->currentState.pipeline);
        VkDescriptorSet descSet = dynamic_cast<VulkanBindGroup*>(bindGroup)->getNative();
        
        std::vector<uint32_t> vulkanDynamicOffsets;
        for (auto &&dynamicOffset : dynamicOffsets) {
            vulkanDynamicOffsets.emplace_back(dynamicOffset);
        }

        vkCmdBindDescriptorSets(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            dynamic_cast<VulkanPipelineLayout*>(pipeline->getDesc().layout)->getNative(),
            index,
            1,
            &descSet,
            static_cast<uint32_t>(vulkanDynamicOffsets.size()),
            vulkanDynamicOffsets.data()
        );
    }

    void VulkanRenderPassEncoder::setBindGroup(std::vector<BindGroup*> bindGroups, std::vector<Uint32> dynamicOffsets = {}) {
        VulkanRenderPipeline* pipeline = dynamic_cast<VulkanRenderPipeline*>(this->currentState.pipeline);
        
        std::vector<uint32_t> vulkanDynamicOffsets;
        for (auto &&dynamicOffset : dynamicOffsets) {
            vulkanDynamicOffsets.emplace_back(dynamicOffset);
        }
        
        std::vector<VkDescriptorSet> vulkanDescSets;
        for (auto &&bindGroup : bindGroups) {
            vulkanDescSets.emplace_back(
                dynamic_cast<VulkanBindGroup*>(bindGroup)->getNative()
            );
        }

        vkCmdBindDescriptorSets(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            dynamic_cast<VulkanPipelineLayout*>(pipeline->getDesc().layout)->getNative(),
            0,
            static_cast<uint32_t>(vulkanDescSets.size()),
            vulkanDescSets.data(),
            static_cast<uint32_t>(vulkanDynamicOffsets.size()),
            vulkanDynamicOffsets.data()
        );
    }

    void VulkanRenderPassEncoder::setIndexBuffer(Buffer* buffer, Uint64 offset) {
        vkCmdBindIndexBuffer(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            dynamic_cast<VulkanBuffer*>(buffer)->getNative(),
            offset,
            convertIndexFormatIntoVulkan(this->currentState.pipeline->getDesc().primitive.stripIndexFormat)
        );
    }

    void VulkanRenderPassEncoder::setVertexBuffer(std::vector<Buffer*> buffers, std::vector<Uint64> offsets = {}) {
        std::vector<uint64_t> vulkanOffsets;

        if (offsets.empty()) {
            for (size_t i = 0; i < buffers.size(); i++) {
                vulkanOffsets.emplace_back(0);
            }
        } else {
            for (auto &&offset : offsets) {
                vulkanOffsets.emplace_back(offset);
            }
        }

        std::vector<VkBuffer> vulkanBuffers;
        for (auto &&buffer : buffers) {
            vulkanBuffers.emplace_back(
                dynamic_cast<VulkanBuffer*>(buffer)->getNative()
            );
        }

        vkCmdBindVertexBuffers(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            0, 
            static_cast<uint32_t>(buffers.size()),
            vulkanBuffers.data(),
            vulkanOffsets.data()
        );
    }

    void VulkanRenderPassEncoder::draw(Uint32 vertexCount, Uint32 instanceCount,  Uint32 firstVertex, Uint32 firstInstance) {
        vkCmdDraw(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            vertexCount,
            instanceCount,
            firstVertex,
            firstInstance
        );
    }

    void VulkanRenderPassEncoder::drawIndexed(Uint32 indexCount, Uint32 instanceCount, Uint32 firstIndex, Int32 baseVertex, Uint32 firstInstance) {
        vkCmdDrawIndexed(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            indexCount,
            instanceCount,
            firstIndex,
            baseVertex,
            firstInstance
        );
    }

    void VulkanRenderPassEncoder::drawIndirect(Buffer* indirectBuffer, Uint64 indirectOffset, Uint64 drawCount) {
        vkCmdDrawIndirect(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            dynamic_cast<VulkanBuffer*>(indirectBuffer)->getNative(),
            indirectOffset,
            drawCount,
            sizeof(VkDrawIndirectCommand)
        );
    }

    void VulkanRenderPassEncoder::drawIndirectCount(Buffer* indirectBuffer, Uint64 indirectOffset = 0, Buffer* countBuffer, Uint64 countOffset = 0) {
        vkCmdDrawIndirectCount(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            dynamic_cast<VulkanBuffer*>(indirectBuffer)->getNative(),
            indirectOffset,
            dynamic_cast<VulkanBuffer*>(countBuffer)->getNative(),
            countOffset,
            this->desc.maxDrawCount,
            sizeof(VkDrawIndirectCommand)
        );
    }

    void VulkanRenderPassEncoder::drawIndexedIndirect(Buffer* indirectBuffer, Uint64 indirectOffset, Uint64 drawCount) {
        vkCmdDrawIndexedIndirect(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            dynamic_cast<VulkanBuffer*>(indirectBuffer)->getNative(),
            indirectOffset,
            drawCount,
            sizeof(VkDrawIndexedIndirectCommand)
        );
    }

    void VulkanRenderPassEncoder::drawIndexedIndirectCount(Buffer* indirectBuffer, Uint64 indirectOffset = 0, Buffer* countBuffer, Uint64 countOffset = 0) {
        vkCmdDrawIndexedIndirectCount(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            dynamic_cast<VulkanBuffer*>(indirectBuffer)->getNative(),
            indirectOffset,
            dynamic_cast<VulkanBuffer*>(countBuffer)->getNative(),
            countOffset,
            this->desc.maxDrawCount,
            sizeof(VkDrawIndexedIndirectCommand)
        );
    }
}