#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    VkDescriptorType convertBindTypeIntoVulkan(BindingType type) {
        switch (type) {
            case BindingType::eUniformBuffer : return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case BindingType::eStorageBuffer : return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case BindingType::eSampledTexture : return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case BindingType::eStorageTexture : return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            
            default: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
    }

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

    std::shared_ptr<BindGroupLayout> VulkanDevice::createBindGroupLayout(BindGroupLayoutDescriptor desc) {
        VkDescriptorSetLayout descSetLayout;
        std::vector<VkDescriptorSetLayoutBinding> bindLayouts;

        for (auto &&entry : desc.entries) {
            VkDescriptorSetLayoutBinding bindLayout;
            bindLayout.binding = entry.binding;
            bindLayout.descriptorType = convertBindTypeIntoVulkan(entry.type);
            bindLayout.stageFlags = convertShaderStageIntoVulkan(entry.shaderStage);
            bindLayout.descriptorCount = entry.bindCount;

            bindLayouts.emplace_back(bindLayout);
        }

        VkDescriptorSetLayoutCreateInfo descLayoutInfo;
        descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descLayoutInfo.bindingCount = static_cast<Uint32>(bindLayouts.size());
        descLayoutInfo.pBindings = bindLayouts.data();
        
        if (vkCreateDescriptorSetLayout(this->device, &descLayoutInfo, nullptr, 
                                        &descSetLayout) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create bind group layout!");
        }

        return std::make_shared<VulkanBindGroupLayout>(desc, this, descSetLayout);
    }

    VulkanBindGroupLayout::~VulkanBindGroupLayout() {
        vkDestroyDescriptorSetLayout(this->device->getNative(), this->descSetLayout, nullptr);
    }
}