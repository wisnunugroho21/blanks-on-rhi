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

        std::map<Uint32, BindGroupLayoutEntry> layoutEntries = desc.layout->getDesc().entries;
        std::vector<VkWriteDescriptorSet> writeDescSets;
        
        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
        std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;

        for (auto &&entry : layoutEntries) {
            VkWriteDescriptorSet writeDescSet;

            writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescSet.dstSet = descSet;
            writeDescSet.dstBinding = entry.second.binding;
            
            writeDescSet.descriptorType = convertBindTypeIntoVulkan(entry.second.type);

            if (entry.second.type == BindingType::eUniformBuffer || entry.second.type == BindingType::eStorageBuffer) {
                BufferBindGroupEntry bufferEntry = desc.bufferEntries[entry.first];

                writeDescSet.descriptorCount = static_cast<uint32_t>(bufferEntry.groupItems.size());
                bufferInfos.emplace_back(std::vector<VkDescriptorBufferInfo>{});

                for (uint32_t i = 0; i < writeDescSet.descriptorCount; i++) {
                    VkDescriptorBufferInfo bufferInfo{};

                    bufferInfo.buffer = static_cast<VulkanBuffer*>(bufferEntry.groupItems[i].buffer)->getNative();
                    bufferInfo.range = bufferEntry.groupItems[i].size == ULLONG_MAX ? VK_WHOLE_SIZE : bufferEntry.groupItems[i].size;
                    bufferInfo.offset = bufferEntry.groupItems[i].offset;

                    bufferInfos[bufferInfos.size() - 1].emplace_back(bufferInfo);
                }

                writeDescSet.pBufferInfo = bufferInfos[bufferInfos.size() - 1].data();
            }

            else if (entry.second.type == BindingType::eSampledTexture || entry.second.type == BindingType::eStorageTexture) {
                TextureBindGroupEntry textureEntry = desc.textureEntries[entry.first];

                writeDescSet.descriptorCount = static_cast<Uint32>(textureEntry.groupItems.size());
                imageInfos.emplace_back(std::vector<VkDescriptorImageInfo>{});

                for (size_t i = 0; i < writeDescSet.descriptorCount; i++) {
                    VulkanTextureView* txtView = dynamic_cast<VulkanTextureView*>(textureEntry.groupItems[i].textureView);
                    VkDescriptorImageInfo imageInfo{};

                    imageInfo.imageView = txtView->getNative();
                    imageInfo.imageLayout = (txtView->getTexture()->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
                        ? VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
                        : VK_IMAGE_LAYOUT_GENERAL;

                    imageInfos[imageInfos.size() - 1].emplace_back(imageInfo);
                }

                writeDescSet.pImageInfo = imageInfos[imageInfos.size() - 1].data();
            }

            else if (entry.second.type == BindingType::eSampler) {
                SamplerBindGroupEntry samplerEntry = desc.samplerEntries[entry.first];

                writeDescSet.descriptorCount = static_cast<Uint32>(samplerEntry.groupItems.size());
                imageInfos.emplace_back(std::vector<VkDescriptorImageInfo>{});

                for (size_t i = 0; i < writeDescSet.descriptorCount; i++) {
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.sampler = dynamic_cast<VulkanSampler*>(samplerEntry.groupItems[i].sampler)->getNative();

                    imageInfos[imageInfos.size() - 1].emplace_back(imageInfo);
                }

                writeDescSet.pImageInfo = imageInfos[imageInfos.size() - 1].data();
            }

            writeDescSets.emplace_back(writeDescSet);
        }

        vkUpdateDescriptorSets(this->device, static_cast<Uint32>(writeDescSets.size()), 
            writeDescSets.data(), 0, nullptr);

        return std::make_shared<VulkanBindGroup>(desc, this, descSet);
    }
}