#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    VkDescriptorType convertBindTypeIntoVulkan(BindingType type) {
        switch (type) {
            case BindingType::eUniformBuffer : return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case BindingType::eStorageBuffer : return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case BindingType::eSampledTexture : return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case BindingType::eStorageTexture : return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case BindingType::eSampler : return VK_DESCRIPTOR_TYPE_SAMPLER;
            
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

    std::shared_ptr<BindGroup> VulkanDevice::createBindGroup(BindGroupDescriptor desc) {
        VkDescriptorSet descSet;

        VkDescriptorSetAllocateInfo descSetAllocInfo{};
        descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descSetAllocInfo.descriptorPool = this->getDescriptorPool();
        descSetAllocInfo.descriptorSetCount = 1;

        if (vkAllocateDescriptorSets(this->device, &descSetAllocInfo, &descSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor set!");
        }

        std::vector<VkWriteDescriptorSet> writeDescSets(desc.entries.size());
        std::vector<BindGroupLayoutEntry> layoutEntries = desc.layout->getDesc().entries;

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;

        for (uint8_t i = 0; i < desc.entries.size(); i++) {
            BindGroupEntry* entry = desc.entries[i];

            int8_t layoutIndex = -1;
            for (int8_t j = 0; j < layoutEntries.size(); j++) {
                if (entry->binding == layoutEntries[j].binding) {
                    layoutIndex = j;
                    break;
                }
            }

            if (layoutIndex < 0) {
                throw std::runtime_error("No binding in group founded in layout");
            }

            writeDescSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescSets[i].dstSet = descSet;
            writeDescSets[i].dstBinding = entry->binding;
            writeDescSets[i].descriptorCount = 1;
            writeDescSets[i].descriptorType = convertBindTypeIntoVulkan(layoutEntries[layoutIndex].type);

            BufferBindGroupEntry* bufferEntry = dynamic_cast<BufferBindGroupEntry*>(entry);

            if (bufferEntry != nullptr) {
                VkDescriptorBufferInfo bufferInfo;
                bufferInfo.buffer = static_cast<VulkanBuffer*>(bufferEntry->buffer)->getNative();
                bufferInfo.range = bufferEntry->size == ULLONG_MAX ? VK_WHOLE_SIZE : bufferEntry->size;
                bufferInfo.offset = bufferEntry->offset;

                bufferInfos.emplace_back(bufferInfo);
                writeDescSets[i].pBufferInfo = &bufferInfos[bufferInfos.size() - 1];

                continue;
            }

            TextureBindGroupEntry* textureEntry = dynamic_cast<TextureBindGroupEntry*>(entry);

            if (textureEntry != nullptr) {
                VulkanTextureView* txtView = dynamic_cast<VulkanTextureView*>(textureEntry->textureView);

                VkDescriptorImageInfo imageInfo;
                imageInfo.imageView = txtView->getNative();
                imageInfo.imageLayout = (txtView->getTexture()->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
                    ? VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
                    : VK_IMAGE_LAYOUT_GENERAL;

                imageInfos.emplace_back(imageInfo);
                writeDescSets[i].pImageInfo = &imageInfos[imageInfos.size() - 1];

                continue;
            }

            SamplerBindGroupEntry* samplerEntry = dynamic_cast<SamplerBindGroupEntry*>(entry);

            if (textureEntry != nullptr) {
                VkDescriptorImageInfo imageInfo;
                imageInfo.sampler = dynamic_cast<VulkanSampler*>(samplerEntry->sampler)->getNative();

                imageInfos.emplace_back(imageInfo);
                writeDescSets[i].pImageInfo = &imageInfos[imageInfos.size() - 1];

                continue;
            }
        }

        vkUpdateDescriptorSets(this->device, static_cast<Uint32>(writeDescSets.size()), 
            writeDescSets.data(), 0, nullptr);

        return std::make_shared<BindGroup>(desc, this, descSet);
    }
}