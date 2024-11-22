#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<ComputePassEncoder> VulkanCommandEncoder::beginComputePass(ComputePassDescriptor desc) {
        return std::make_shared<VulkanComputePassEncoder>(desc, this);
    }

    void VulkanComputePassEncoder::setPipeline(ComputePipeline* pipeline) {
        vkCmdBindPipeline(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_PIPELINE_BIND_POINT_COMPUTE,
            dynamic_cast<VulkanComputePipeline*>(pipeline)->getNative()
        );

        this->currentComputeState.pipeline = pipeline;
    }

    void VulkanComputePassEncoder::setBindGroup(BindGroup* bindGroup, std::vector<Uint32> dynamicOffsets) {
        VulkanRenderPipeline* pipeline = dynamic_cast<VulkanRenderPipeline*>(this->currentComputeState.pipeline);
        VkDescriptorSet descSet = dynamic_cast<VulkanBindGroup*>(bindGroup)->getNative();
        
        std::vector<uint32_t> vulkanDynamicOffsets;
        for (auto &&dynamicOffset : dynamicOffsets) {
            vulkanDynamicOffsets.emplace_back(dynamicOffset);
        }

        vkCmdBindDescriptorSets(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            VK_PIPELINE_BIND_POINT_COMPUTE,
            dynamic_cast<VulkanPipelineLayout*>(pipeline->getDesc().layout)->getNative(),
            0,
            1,
            &descSet,
            static_cast<uint32_t>(vulkanDynamicOffsets.size()),
            vulkanDynamicOffsets.data()
        );

        this->currentComputeState.bindGroups.clear();
        this->currentComputeState.bindGroups.emplace_back(bindGroup);

        this->currentComputeState.dynamicOffsets = dynamicOffsets;
    }

    void VulkanComputePassEncoder::setBindGroup(std::vector<BindGroup*> bindGroups, std::vector<Uint32> dynamicOffsets) {
        VulkanRenderPipeline* pipeline = dynamic_cast<VulkanRenderPipeline*>(this->currentComputeState.pipeline);
        
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
            VK_PIPELINE_BIND_POINT_COMPUTE,
            dynamic_cast<VulkanPipelineLayout*>(pipeline->getDesc().layout)->getNative(),
            0,
            static_cast<uint32_t>(vulkanDescSets.size()),
            vulkanDescSets.data(),
            static_cast<uint32_t>(vulkanDynamicOffsets.size()),
            vulkanDynamicOffsets.data()
        );

        this->currentComputeState.bindGroups = bindGroups;
        this->currentComputeState.dynamicOffsets = dynamicOffsets;
    }

    void VulkanComputePassEncoder::dispatchWorkgroups(Uint32 workgroupCountX, Uint32 workgroupCountY, Uint32 workgroupCountZ) {
        vkCmdDispatch(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            workgroupCountX,
            workgroupCountY,
            workgroupCountZ
        );
    }

    void VulkanComputePassEncoder::dispatchWorkgroupsIndirect(Buffer* indirectBuffer, Uint64 indirectOffset) {
        vkCmdDispatchIndirect(
            dynamic_cast<VulkanCommandEncoder*>(this->commandEncoder)->getNative(),
            dynamic_cast<VulkanBuffer*>(indirectBuffer)->getNative(),
            indirectOffset
        );
    }

    void VulkanComputePassEncoder::end() {

    }
}