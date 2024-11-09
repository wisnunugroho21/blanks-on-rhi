#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    std::shared_ptr<ComputePipeline> VulkanDevice::createComputePipeline(ComputePipelineDescriptor desc) {
        VkPipeline computePipeline;

        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageInfo.module = dynamic_cast<VulkanShaderModule*>(desc.compute.module)->getNative();

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.stage = shaderStageInfo;
        pipelineInfo.layout = dynamic_cast<VulkanPipelineLayout*>(desc.layout)->getNative();

        if (vkCreateComputePipelines(this->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline!");
        }

        return std::make_shared<VulkanComputePipeline>(desc, this, computePipeline);
    }

    BindGroupLayout* VulkanComputePipeline::getBindGroupLayout(Uint32 index) {
        return this->desc.layout->getDesc().bindGroupLayouts[index];
    }

    VulkanComputePipeline::~VulkanComputePipeline() {
        vkDestroyPipeline(this->device->getNative(), this->pipeline, nullptr);
    }
}