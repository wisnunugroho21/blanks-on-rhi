#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<BindGroupLayout> VulkanDevice::createBindGroupLayout(BindGroupLayoutDescriptor desc) {
        std::vector<VkDescriptorSetLayoutBinding> bindLayouts;

        for (auto &&entry : desc.entries) {
            VkDescriptorSetLayoutBinding bindLayout {
                .binding = entry.second.binding,
                .descriptorType = convertBindTypeIntoVulkan(entry.second.type),
                .stageFlags = convertShaderStageIntoVulkan(entry.second.shaderStage),
                .descriptorCount = entry.second.bindCount
            };

            bindLayouts.emplace_back(bindLayout);
        }

        VkDescriptorSetLayoutCreateInfo descLayoutInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<Uint32>(bindLayouts.size()),
            .pBindings = bindLayouts.data()
        };
        
        VkDescriptorSetLayout descSetLayout;
        if (vkCreateDescriptorSetLayout(this->device, &descLayoutInfo, nullptr, &descSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create bind group layout!");
        }

        return std::make_shared<VulkanBindGroupLayout>(desc, this, descSetLayout);
    }

    VulkanBindGroupLayout::~VulkanBindGroupLayout() {
        vkDestroyDescriptorSetLayout(this->device->getNative(), this->descSetLayout, nullptr);
    }

    std::shared_ptr<BindGroup> VulkanDevice::createBindGroup(BindGroupDescriptor desc) {
        VkDescriptorSetAllocateInfo descSetAllocInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = this->getDescriptorPool(),
            .descriptorSetCount = 1
        };

        VkDescriptorSet descSet;
        if (vkAllocateDescriptorSets(this->device, &descSetAllocInfo, &descSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor set!");
        }

        std::map<Uint32, BindGroupLayoutEntry> layoutEntries = desc.layout->getDesc().entries;
        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
        std::vector<std::vector<VkDescriptorImageInfo>> imageInfos;

        std::vector<VkWriteDescriptorSet> writeDescSets;
        for (auto &&entry : layoutEntries) {
            VkWriteDescriptorSet writeDescSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descSet,
                .dstBinding = entry.second.binding,
                .descriptorType = convertBindTypeIntoVulkan(entry.second.type)
            };

            if (entry.second.type == BindingType::eUniformBuffer || entry.second.type == BindingType::eStorageBuffer) {
                BufferBindGroupEntry bufferEntry = desc.bufferEntries[entry.first];

                writeDescSet.descriptorCount = static_cast<uint32_t>(bufferEntry.groupItems.size());
                std::vector<VkDescriptorBufferInfo> curBufferInfos{};

                for (auto &&groupItem : bufferEntry.groupItems) {
                    VkDescriptorBufferInfo bufferInfo{
                        .buffer = static_cast<VulkanBuffer*>(groupItem.buffer)->getNative(),
                        .range = groupItem.size == ULLONG_MAX ? VK_WHOLE_SIZE : groupItem.size,
                        .offset = groupItem.offset
                    };

                    curBufferInfos.emplace_back(bufferInfo);
                }

                bufferInfos.emplace_back(curBufferInfos);
                writeDescSet.pBufferInfo = bufferInfos[bufferInfos.size() - 1].data();
            }

            else if (entry.second.type == BindingType::eSampledTexture || entry.second.type == BindingType::eStorageTexture) {
                TextureBindGroupEntry textureEntry = desc.textureEntries[entry.first];

                writeDescSet.descriptorCount = static_cast<Uint32>(textureEntry.groupItems.size());
                std::vector<VkDescriptorImageInfo> curImageInfos{};

                for (auto &&groupItem : textureEntry.groupItems) {
                    VulkanTextureView* txtView = dynamic_cast<VulkanTextureView*>(groupItem.textureView);
                    VkDescriptorImageInfo imageInfo{};

                    imageInfo.imageView = txtView->getNative();
                    imageInfo.imageLayout = (txtView->getTexture()->getDesc().usage & std::to_underlying(TextureUsage::eTextureBinding)) 
                        ? VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
                        : VK_IMAGE_LAYOUT_GENERAL;

                    curImageInfos.emplace_back(imageInfo);
                }

                imageInfos.emplace_back(curImageInfos);
                writeDescSet.pImageInfo = imageInfos[imageInfos.size() - 1].data();
            }

            else if (entry.second.type == BindingType::eSampler) {
                SamplerBindGroupEntry samplerEntry = desc.samplerEntries[entry.first];

                writeDescSet.descriptorCount = static_cast<Uint32>(samplerEntry.groupItems.size());
                std::vector<VkDescriptorImageInfo> curImageInfos{};

                for (auto &&groupItem : samplerEntry.groupItems) {
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.sampler = dynamic_cast<VulkanSampler*>(groupItem.sampler)->getNative();

                    curImageInfos.emplace_back(imageInfo);
                }

                imageInfos.emplace_back(curImageInfos);
                writeDescSet.pImageInfo = imageInfos[imageInfos.size() - 1].data();
            }

            writeDescSets.emplace_back(writeDescSet);
        }

        vkUpdateDescriptorSets(this->device, static_cast<Uint32>(writeDescSets.size()), 
            writeDescSets.data(), 0, nullptr);

        return std::make_shared<VulkanBindGroup>(desc, this, descSet);
    }
}