#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    VkShaderStageFlags convertShaderStageIntoVulkan(ShaderStageFlags stage) {
        VkShaderStageFlags shaderStages = 0;

        if (stage & std::to_underlying(ShaderStage::eCompute)) {
            shaderStages |= VK_SHADER_STAGE_COMPUTE_BIT;
        }

        if (stage & std::to_underlying(ShaderStage::eVertex)) {
            shaderStages |= VK_SHADER_STAGE_VERTEX_BIT;
        }

        if (stage & std::to_underlying(ShaderStage::eFragment)) {
            shaderStages |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        if (stage & std::to_underlying(ShaderStage::eTessellCtrl)) {
            shaderStages |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        }

        if (stage & std::to_underlying(ShaderStage::eTessellEval)) {
            shaderStages |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }

        if (stage & std::to_underlying(ShaderStage::eTask)) {
            shaderStages |= VK_SHADER_STAGE_TASK_BIT_EXT;
        }

        if (stage & std::to_underlying(ShaderStage::eMesh)) {
            shaderStages |= VK_SHADER_STAGE_MESH_BIT_EXT;
        }

        return shaderStages;
    }

    std::shared_ptr<PipelineLayout> VulkanDevice::createPipelineLayout(PipelineLayoutDescriptor desc) {
        VkPipelineLayout pipelineLayout;

        std::vector<VkDescriptorSetLayout> vulkanDescLayouts;
        std::vector<VkPushConstantRange> vulkanPushConstants;

        for (auto &&bindGroupLayout : desc.bindGroupLayouts) {
            vulkanDescLayouts.emplace_back(dynamic_cast<VulkanBindGroupLayout*>(bindGroupLayout)->getNative());
        }

        for (auto &&constantLayout : desc.constantLayouts) {
            VkPushConstantRange pushConstant{};
            pushConstant.stageFlags = convertShaderStageIntoVulkan(constantLayout.shaderStage);
            pushConstant.size = constantLayout.size;
            pushConstant.offset = constantLayout.offset;
        }

        VkPipelineLayoutCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineInfo.setLayoutCount = static_cast<uint32_t>(vulkanDescLayouts.size());
        pipelineInfo.pSetLayouts = vulkanDescLayouts.data();
        pipelineInfo.pushConstantRangeCount = static_cast<uint32_t>(vulkanPushConstants.size());
        pipelineInfo.pPushConstantRanges = vulkanPushConstants.data();

        if (vkCreatePipelineLayout(this->device, &pipelineInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        return std::make_shared<VulkanPipelineLayout>(desc, this, pipelineLayout);
    }

    VulkanPipelineLayout::~VulkanPipelineLayout() {
        vkDestroyPipelineLayout(this->device->getNative(), this->pipelineLayout, nullptr);
    }
}