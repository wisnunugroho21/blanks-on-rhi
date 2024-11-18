#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    std::shared_ptr<BindGroupLayout> VulkanDevice::createBindGroupLayout(BindGroupLayoutDescriptor desc) {
        VkDescriptorSetLayout descSetLayout;
        std::vector<VkDescriptorSetLayoutBinding> bindLayouts;

        for (auto &&entry : desc.entries) {
            VkDescriptorSetLayoutBinding bindLayout;
            bindLayout.binding = entry.second.binding;
            bindLayout.descriptorType = convertBindTypeIntoVulkan(entry.second.type);
            bindLayout.stageFlags = convertShaderStageIntoVulkan(entry.second.shaderStage);
            bindLayout.descriptorCount = entry.second.bindCount;

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

        std::vector<VkWriteDescriptorSet> writeDescSets;

        std::map<Uint32, BindGroupLayoutEntry> layoutEntries = desc.layout->getDesc().entries;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;

        for (auto &&entry : desc.entries) {
            VkWriteDescriptorSet writeDescSet;

            writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescSet.dstSet = descSet;
            writeDescSet.dstBinding = entry.second->binding;
            
            writeDescSet.descriptorType = convertBindTypeIntoVulkan(layoutEntries[entry.first].type);

            BufferBindGroupEntry* bufferEntry = dynamic_cast<BufferBindGroupEntry*>(entry.second);

            if (bufferEntry != nullptr) {
                writeDescSet.descriptorCount = static_cast<Uint32>(bufferEntry->groupItems.size());
                std::vector<VkDescriptorBufferInfo> bufferInfos { writeDescSet.descriptorCount };

                for (size_t i = 0; i < bufferInfos.size(); i++) {
                    bufferInfos[i].buffer = static_cast<VulkanBuffer*>(bufferEntry->groupItems[i].buffer)->getNative();
                    bufferInfos[i].range = bufferEntry->groupItems[i].size == ULLONG_MAX ? VK_WHOLE_SIZE : bufferEntry->groupItems[i].size;
                    bufferInfos[i].offset = bufferEntry->groupItems[i].offset;
                }

                writeDescSet.pBufferInfo = bufferInfos.data();
                continue;
            }

            TextureBindGroupEntry* textureEntry = dynamic_cast<TextureBindGroupEntry*>(entry.second);

            if (textureEntry != nullptr) {
                writeDescSet.descriptorCount = static_cast<Uint32>(textureEntry->groupItems.size());
                std::vector<VkDescriptorImageInfo> imageInfos { writeDescSet.descriptorCount };

                for (size_t i = 0; i < imageInfos.size(); i++) {
                    VulkanTextureView* txtView = dynamic_cast<VulkanTextureView*>(textureEntry->groupItems[i].textureView);

                    imageInfos[i].imageView = txtView->getNative();
                    imageInfos[i].imageLayout = (txtView->getTexture()->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
                        ? VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
                        : VK_IMAGE_LAYOUT_GENERAL;
                }

                writeDescSet.pImageInfo = imageInfos.data();
                continue;
            }

            SamplerBindGroupEntry* samplerEntry = dynamic_cast<SamplerBindGroupEntry*>(entry.second);

            if (samplerEntry != nullptr) {
                writeDescSet.descriptorCount = static_cast<Uint32>(samplerEntry->groupItems.size());
                std::vector<VkDescriptorImageInfo> imageInfos { writeDescSet.descriptorCount };

                for (size_t i = 0; i < imageInfos.size(); i++) {
                    imageInfos[i].sampler = dynamic_cast<VulkanSampler*>(samplerEntry->groupItems[i].sampler)->getNative();
                }

                writeDescSet.pImageInfo = imageInfos.data();
                continue;
            }

            writeDescSets.emplace_back(writeDescSet);
        }

        vkUpdateDescriptorSets(this->device, static_cast<Uint32>(writeDescSets.size()), 
            writeDescSets.data(), 0, nullptr);

        return std::make_shared<VulkanBindGroup>(desc, this, descSet);
    }
}