#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<RenderGraph> VulkanDevice::createRenderGraph(RenderGraphDescriptor desc) {
        std::vector<VulkanRenderPassNode> renderPassNodes{ desc.renderPasses.size() };

        for (size_t i = 0; i < desc.renderPasses.size(); i++) {
            RenderPassDescriptor renderPassDesc = desc.renderPasses[i];

            std::vector<VkAttachmentDescription> attachmentsDescs;
            std::vector<VkAttachmentReference> colorAttachmentRefs;
            std::vector<VkAttachmentReference> resolveAttachmentRefs;
            
            uint32_t attachmentIndex = 0;
            for (size_t j = 0; j < renderPassDesc.colorAttachments.size(); j++) {
                RenderPassColorAttachment colorAttachment = renderPassDesc.colorAttachments[j];

                attachmentsDescs.emplace_back(VkAttachmentDescription{
                    .format = convertTextureFormatIntoVulkan(colorAttachment.format),
                    .samples = convertSampleCountIntoVulkan(renderPassDesc.sampleCount),
                    .loadOp = convertLoadOpIntoVulkan(colorAttachment.loadOp),
                    .storeOp = convertStoreOpIntoVulkan(colorAttachment.storeOp),
                    .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                });

                colorAttachmentRefs.emplace_back(VkAttachmentReference{
                    .attachment = attachmentIndex++,
                    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                });

                if (renderPassDesc.sampleCount > 1u) {
                    attachmentsDescs.emplace_back(VkAttachmentDescription{
                        .format = convertTextureFormatIntoVulkan(colorAttachment.format),
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .loadOp = convertLoadOpIntoVulkan(colorAttachment.loadOp),
                        .storeOp = convertStoreOpIntoVulkan(colorAttachment.storeOp),
                        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    });

                    resolveAttachmentRefs.emplace_back(VkAttachmentReference{
                        .attachment = attachmentIndex++,
                        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    });
                }
            }

            VkSubpassDescription subpassDesc{
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size()),
                .pColorAttachments = colorAttachmentRefs.data(),
                .pResolveAttachments = resolveAttachmentRefs.data()
            };

            VkAttachmentReference depthStencilAttachmentRef{
                .attachment = attachmentIndex,
                .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

            if (renderPassDesc.depthStencilAttachment.format != TextureFormat::eUndefined) {
                attachmentsDescs.emplace_back(VkAttachmentDescription{
                    .format = convertTextureFormatIntoVulkan(renderPassDesc.depthStencilAttachment.format),
                    .samples = convertSampleCountIntoVulkan(renderPassDesc.sampleCount),
                    .loadOp = convertLoadOpIntoVulkan(renderPassDesc.depthStencilAttachment.depthLoadOp),
                    .storeOp = convertStoreOpIntoVulkan(renderPassDesc.depthStencilAttachment.depthStoreOp),
                    .stencilLoadOp = convertLoadOpIntoVulkan(renderPassDesc.depthStencilAttachment.stencilLoadOp),
                    .stencilStoreOp = convertStoreOpIntoVulkan(renderPassDesc.depthStencilAttachment.stencilStoreOp),
                    .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                });

                subpassDesc.pDepthStencilAttachment = &depthStencilAttachmentRef;
            }

            VkRenderPassCreateInfo renderPassInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = static_cast<uint32_t>(attachmentsDescs.size()),
                .pAttachments = attachmentsDescs.data(),
                .subpassCount = 1,
                .pSubpasses = &subpassDesc,
                .dependencyCount = 0,
                .pDependencies = nullptr
            };

            if (vkCreateRenderPass(this->device, &renderPassInfo, nullptr, &renderPassNodes[i].renderPass) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create render pass!");
            }

            renderPassNodes[i].pipelineNodes.resize(renderPassDesc.renderPipelines.size());

            for (size_t j = 0; j < renderPassDesc.renderPipelines.size(); j++) {
                RenderPipelineDescriptor renderPipelineDesc = renderPassDesc.renderPipelines[j];
                renderPassNodes[i].pipelineNodes[j].bindGroupNodes.resize(renderPipelineDesc.bindGroupLayouts.size());

                for (size_t k = 0; k < renderPipelineDesc.bindGroupLayouts.size(); i++) {
                    BindGroupLayoutDescriptor bindGroupLayoutDesc = renderPipelineDesc.bindGroupLayouts[k];
                    std::vector<VkDescriptorSetLayoutBinding> bindLayouts;

                    for (auto &&entry : bindGroupLayoutDesc.entries) {
                        VkDescriptorSetLayoutBinding bindLayout {
                            .binding = entry.binding,
                            .descriptorType = convertBindTypeIntoVulkan(entry.type),
                            .stageFlags = convertShaderStageIntoVulkan(entry.shaderStage),
                            .descriptorCount = entry.bindCount
                        };

                        bindLayouts.emplace_back(bindLayout);
                    }

                    VkDescriptorSetLayoutCreateInfo descLayoutInfo {
                        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                        .bindingCount = static_cast<Uint32>(bindLayouts.size()),
                        .pBindings = bindLayouts.data()
                    };
                    
                    if (vkCreateDescriptorSetLayout(this->device, &descLayoutInfo, nullptr, 
                        &renderPassNodes[i].pipelineNodes[j].bindGroupNodes[k].bindGroupLayout) != VK_SUCCESS) 
                    {
                        throw std::runtime_error("Failed to create bind group layout!");
                    }
                }

                std::vector<VkDescriptorSetLayout> vulkanDescLayouts;

                for (auto &&node : renderPassNodes[i].pipelineNodes[j].bindGroupNodes) {
                    vulkanDescLayouts.emplace_back(node.bindGroupLayout);
                }

                VkPipelineLayoutCreateInfo pipelineLayoutInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                    .setLayoutCount = static_cast<uint32_t>(vulkanDescLayouts.size()),
                    .pSetLayouts = vulkanDescLayouts.data(),
                    .pushConstantRangeCount = 0,
                    .pPushConstantRanges = nullptr
                };

                if (vkCreatePipelineLayout(this->device, &pipelineLayoutInfo, nullptr, 
                    &renderPassNodes[i].pipelineNodes[j].pipelineLayout) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to create pipeline layout!");
                }

                std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageInfos{
                    VkPipelineShaderStageCreateInfo {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .stage = VK_SHADER_STAGE_VERTEX_BIT,
                        .module = dynamic_cast<VulkanShaderModule*>(renderPipelineDesc.vertex.module)->getNative()
                    },

                    VkPipelineShaderStageCreateInfo {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                        .module = dynamic_cast<VulkanShaderModule*>(renderPipelineDesc.fragment.module)->getNative()
                    }
                };

                std::vector<VkVertexInputBindingDescription> vertexBindingInfos{};
                std::vector<VkVertexInputAttributeDescription> vertexAttributeInfos{};

                for (uint32_t i = 0; i < renderPipelineDesc.vertex.buffers.size(); i++) {
                    VertexBufferLayout vertexBufferLayout = renderPipelineDesc.vertex.buffers[i];

                    VkVertexInputBindingDescription vertexBindingInfo{
                        .binding = i,
                        .stride = vertexBufferLayout.arrayStride,
                        .inputRate = convertVertexStepModeIntoVulkan(vertexBufferLayout.stepMode)
                    };

                    vertexBindingInfos.emplace_back(vertexBindingInfo);
                    
                    for (uint32_t j = 0; j < vertexBufferLayout.attributes.size(); j++) {
                        VertexAttribute vertexAttribute = vertexBufferLayout.attributes[j];

                        VkVertexInputAttributeDescription vertexAttributeInfo{
                            .binding = i,
                            .location = j,
                            .format = convertVertexFormatIntoVulkan(vertexAttribute.format),
                            .offset = vertexAttribute.offset
                        };

                        vertexAttributeInfos.emplace_back(vertexAttributeInfo);
                    }
                }

                VkPipelineVertexInputStateCreateInfo vertexInputInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                    .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingInfos.size()),
                    .pVertexBindingDescriptions = vertexBindingInfos.data(),
                    .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeInfos.size()),
                    .pVertexAttributeDescriptions = vertexAttributeInfos.data()
                };

                VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                    .topology = convertPrimitiveTopologyIntoVulkan(renderPipelineDesc.primitive.topology),
                    .primitiveRestartEnable = VK_FALSE
                };

                std::vector<VkViewport> vulkanViewports{};
                std::vector<VkRect2D> vulkanScissors{};

                for (auto &&viewport : renderPipelineDesc.viewportScissor.viewports) {
                    vulkanViewports.emplace_back(convertViewportIntoVulkan(viewport));
                }

                for (auto &&scissor : renderPipelineDesc.viewportScissor.scissors) {
                    vulkanScissors.emplace_back(convertRect2DIntoVulkan(scissor));
                }

                VkPipelineViewportStateCreateInfo viewportInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                    .viewportCount = static_cast<uint32_t>(vulkanViewports.size()),
                    .pViewports = vulkanViewports.data(),
                    .scissorCount = static_cast<uint32_t>(vulkanScissors.size()),
                    .pScissors = vulkanScissors.data()
                };

                VkPipelineRasterizationStateCreateInfo rasterizationInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                    .depthClampEnable = VK_FALSE,
                    .rasterizerDiscardEnable = VK_FALSE,

                    .frontFace = convertFrontFaceIntoVulkan(renderPipelineDesc.rasterizationState.frontFace),
                    .cullMode = convertCullModeIntoVulkan(renderPipelineDesc.rasterizationState.cullMode),
                    .polygonMode = convertPolygonModeIntoVulkan(renderPipelineDesc.rasterizationState.polygonMode),

                    .depthBiasEnable = renderPipelineDesc.rasterizationState.depthBias.constant > 0.0f ? VK_TRUE : VK_FALSE,
                    .depthBiasConstantFactor = renderPipelineDesc.rasterizationState.depthBias.constant,
                    .depthBiasClamp = renderPipelineDesc.rasterizationState.depthBias.clamp,
                    .depthBiasSlopeFactor = renderPipelineDesc.rasterizationState.depthBias.slopeScale,

                    .lineWidth = 1.0f
                };

                VkSampleMask sampleMask = renderPipelineDesc.multisample.mask;

                VkPipelineMultisampleStateCreateInfo multiSampleInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                    .rasterizationSamples = convertSampleCountIntoVulkan(renderPipelineDesc.multisample.count),

                    .sampleShadingEnable = renderPipelineDesc.multisample.count > 1 ? VK_TRUE : VK_FALSE,
                    .minSampleShading = 1.0f,
                    .pSampleMask = &sampleMask,

                    .alphaToCoverageEnable = renderPipelineDesc.multisample.alphaToCoverageEnabled ? VK_TRUE : VK_FALSE,
                    .alphaToOneEnable = renderPipelineDesc.multisample.alphaToOneEnabled ? VK_TRUE : VK_FALSE,
                };

                VkStencilOpState frontStencil{
                    .failOp = convertStencilOpIntoVulkan(renderPipelineDesc.stencil.stencilFront.failOp),
                    .passOp = convertStencilOpIntoVulkan(renderPipelineDesc.stencil.stencilFront.passOp),
                    .depthFailOp = convertStencilOpIntoVulkan(renderPipelineDesc.stencil.stencilFront.depthFailOp),

                    .compareOp = convertCompareOpToVulkan(renderPipelineDesc.stencil.stencilFront.compareOp),
                    .compareMask = renderPipelineDesc.stencil.stencilCompareMask,

                    .writeMask = renderPipelineDesc.stencil.stencilWriteMask,
                    .reference = renderPipelineDesc.stencil.stencilRefence
                };

                VkStencilOpState backStencil{
                    .failOp = convertStencilOpIntoVulkan(renderPipelineDesc.stencil.stencilBack.failOp),
                    .passOp = convertStencilOpIntoVulkan(renderPipelineDesc.stencil.stencilBack.passOp),
                    .depthFailOp = convertStencilOpIntoVulkan(renderPipelineDesc.stencil.stencilBack.depthFailOp),

                    .compareOp = convertCompareOpToVulkan(renderPipelineDesc.stencil.stencilBack.compareOp),
                    .compareMask = renderPipelineDesc.stencil.stencilCompareMask,
                    
                    .writeMask = renderPipelineDesc.stencil.stencilWriteMask,
                    .reference = renderPipelineDesc.stencil.stencilRefence
                };

                VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                    .depthTestEnable = renderPipelineDesc.depth.depthTestEnabled ? VK_TRUE : VK_FALSE,
                    .depthWriteEnable = renderPipelineDesc.depth.depthWriteEnabled ? VK_TRUE : VK_FALSE,
                    .depthCompareOp = convertCompareOpToVulkan(renderPipelineDesc.depth.depthCompareOp),
                    .depthBoundsTestEnable = renderPipelineDesc.depth.depthBoundsTestEnabled ? VK_TRUE : VK_FALSE,
                    .stencilTestEnable = renderPipelineDesc.stencil.stencilTestEnabled ? VK_TRUE : VK_FALSE,
                    .front = frontStencil,
                    .back = backStencil,
                    .minDepthBounds = renderPipelineDesc.depth.minDepthBounds,
                    .maxDepthBounds = renderPipelineDesc.depth.maxDepthBounds
                };

                std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
                for (auto &&target : renderPipelineDesc.fragment.targets) {
                    VkPipelineColorBlendAttachmentState colorBlendAttachment{
                        .blendEnable = target.blend.blendEnabled ? VK_TRUE : VK_FALSE,
                        .colorWriteMask = convertColorComponentIntoVulkan(target.colorWriteMask),

                        .srcColorBlendFactor = convertBlendFactorIntoVulkan(target.blend.color.srcFactor),
                        .dstColorBlendFactor = convertBlendFactorIntoVulkan(target.blend.color.dstFactor),
                        .colorBlendOp = convertBlendOperationIntoVulkan(target.blend.color.operation),

                        .srcAlphaBlendFactor = convertBlendFactorIntoVulkan(target.blend.alpha.srcFactor),
                        .dstAlphaBlendFactor = convertBlendFactorIntoVulkan(target.blend.alpha.dstFactor),
                        .alphaBlendOp = convertBlendOperationIntoVulkan(target.blend.alpha.operation)
                    };

                    colorBlendAttachments.emplace_back(colorBlendAttachment);
                }

                VkPipelineColorBlendStateCreateInfo colorBlendInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                    .logicOpEnable = renderPipelineDesc.fragment.logicOp != LogicOp::eCopy ? VK_TRUE : VK_FALSE,
                    .logicOp = convertLogicOpIntoVulkan(renderPipelineDesc.fragment.logicOp),
                    .attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
                    .pAttachments = colorBlendAttachments.data(),
                    .blendConstants[0] = renderPipelineDesc.fragment.blendConstant.r,
                    .blendConstants[1] = renderPipelineDesc.fragment.blendConstant.g,
                    .blendConstants[2] = renderPipelineDesc.fragment.blendConstant.b,
                    .blendConstants[3] = renderPipelineDesc.fragment.blendConstant.a
                };

                std::vector<VkDynamicState> dynamicStates = convertDynamicStatesIntoVulkan(renderPipelineDesc.dynamicState);

                VkPipelineDynamicStateCreateInfo dynamicStateInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                    .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
                    .pDynamicStates = dynamicStates.data()
                };

                std::vector<VkFormat> colorAttachmentFormats{};
                for (auto &&target : renderPipelineDesc.fragment.targets) {
                    colorAttachmentFormats.emplace_back(convertTextureFormatIntoVulkan(target.format));
                }

                VkGraphicsPipelineCreateInfo pipelineInfo{
                    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

                    .stageCount = static_cast<uint32_t>(shaderStageInfos.size()),
                    .pStages = shaderStageInfos.data(),

                    .pVertexInputState = &vertexInputInfo,
                    .pInputAssemblyState = &inputAssemblyInfo,
                    .pViewportState = &viewportInfo,
                    .pRasterizationState = &rasterizationInfo,
                    .pMultisampleState = &multiSampleInfo,
                    .pDepthStencilState = &depthStencilInfo,
                    .pColorBlendState = &colorBlendInfo,
                    .pDynamicState = &dynamicStateInfo,

                    .layout = renderPassNodes[i].pipelineNodes[j].pipelineLayout,
                    .renderPass = renderPassNodes[i].renderPass,
                    .subpass = 0
                };

                if (vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, 
                    &renderPassNodes[i].pipelineNodes[j].pipeline) != VK_SUCCESS) 
                {
                    throw std::runtime_error("Failed to create render pipeline!");
                }
            }
        }

        return std::make_shared<VulkanRenderGraph>(desc, this, renderPassNodes);
    }

    VulkanRenderGraph::~VulkanRenderGraph() {
        VkDevice nativeDevice = this->device->getNative();

        for (auto &&renderPassNode : this->renderPassNodes) {
            for (auto &&pipelineNode : renderPassNode.pipelineNodes) {
                for (auto &&bindGroupNode : pipelineNode.bindGroupNodes) {
                    vkDestroyDescriptorSetLayout(nativeDevice, bindGroupNode.bindGroupLayout, nullptr);
                }

                vkDestroyPipelineLayout(nativeDevice, pipelineNode.pipelineLayout, nullptr);
            }

            vkDestroyRenderPass(nativeDevice, renderPassNode.renderPass, nullptr);
        }
    }
}