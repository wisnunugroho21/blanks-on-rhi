#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<PipelineLayout> VulkanDevice::createPipelineLayout(PipelineLayoutDescriptor desc) {
        std::vector<VkDescriptorSetLayout> vulkanDescLayouts;
        std::vector<VkPushConstantRange> vulkanPushConstants;

        for (auto &&bindGroupLayout : desc.bindGroupLayouts) {
            vulkanDescLayouts.emplace_back(dynamic_cast<VulkanBindGroupLayout*>(bindGroupLayout)->getNative());
        }

        for (auto &&constantLayout : desc.constantLayouts) {
            VkPushConstantRange pushConstant{
                .stageFlags = convertShaderStageIntoVulkan(constantLayout.shaderStage),
                .size = constantLayout.size,
                .offset = constantLayout.offset
            };
        }

        VkPipelineLayoutCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(vulkanDescLayouts.size()),
            .pSetLayouts = vulkanDescLayouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(vulkanPushConstants.size()),
            .pPushConstantRanges = vulkanPushConstants.data()
        };

        VkPipelineLayout pipelineLayout;
        if (vkCreatePipelineLayout(this->device, &pipelineInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        return std::make_shared<VulkanPipelineLayout>(desc, this, pipelineLayout);
    }

    VulkanPipelineLayout::~VulkanPipelineLayout() {
        vkDestroyPipelineLayout(this->device->getNative(), this->pipelineLayout, nullptr);
    }
}