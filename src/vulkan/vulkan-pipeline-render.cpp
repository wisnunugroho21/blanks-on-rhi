#include "vulkan-backend.hpp"

namespace RHI {
    std::shared_ptr<RenderPipeline> VulkanDevice::createRenderPipeline(RenderPipelineDescriptor desc) {
        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageInfos{
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = dynamic_cast<VulkanShaderModule*>(desc.vertex.module)->getNative()
            },

            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = dynamic_cast<VulkanShaderModule*>(desc.fragment.module)->getNative()
            }
        };

        std::vector<VkVertexInputBindingDescription> vertexBindingInfoes{};
        std::vector<VkVertexInputAttributeDescription> vertexAttributeInfoes{};

        for (uint32_t i = 0; i < desc.vertex.buffers.size(); i++) {
            VertexBufferLayout vertexBufferLayout = desc.vertex.buffers[i];

            VkVertexInputBindingDescription vertexBindingInfo{
                .binding = i,
                .stride = vertexBufferLayout.arrayStride,
                .inputRate = convertVertexStepModeIntoVulkan(vertexBufferLayout.stepMode)
            };

            vertexBindingInfoes.emplace_back(vertexBindingInfo);
            
            for (uint32_t j = 0; j < vertexBufferLayout.attributes.size(); j++) {
                VertexAttribute vertexAttribute = vertexBufferLayout.attributes[j];

                VkVertexInputAttributeDescription vertexAttributeInfo{
                    .binding = i,
                    .location = j,
                    .format = convertVertexFormatIntoVulkan(vertexAttribute.format),
                    .offset = vertexAttribute.offset
                };

                vertexAttributeInfoes.emplace_back(vertexAttributeInfo);
            }
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingInfoes.size()),
            .pVertexBindingDescriptions = vertexBindingInfoes.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeInfoes.size()),
            .pVertexAttributeDescriptions = vertexAttributeInfoes.data()
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = convertPrimitiveTopologyIntoVulkan(desc.primitive.topology),
            .primitiveRestartEnable = VK_FALSE
        };

        std::vector<VkViewport> vulkanViewports{};
        std::vector<VkRect2D> vulkanScissors{};

        for (auto &&viewport : desc.viewportScissor.viewports) {
            vulkanViewports.emplace_back(convertViewportIntoVulkan(viewport));
        }

        for (auto &&scissor : desc.viewportScissor.scissors) {
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

            .frontFace = convertFrontFaceIntoVulkan(desc.rasterizationState.frontFace),
            .cullMode = convertCullModeIntoVulkan(desc.rasterizationState.cullMode),
            .polygonMode = convertPolygonModeIntoVulkan(desc.rasterizationState.polygonMode),

            .depthBiasEnable = desc.rasterizationState.depthBias.constant > 0.0f ? VK_TRUE : VK_FALSE,
            .depthBiasConstantFactor = desc.rasterizationState.depthBias.constant,
            .depthBiasClamp = desc.rasterizationState.depthBias.clamp,
            .depthBiasSlopeFactor = desc.rasterizationState.depthBias.slopeScale,

            .lineWidth = desc.rasterizationState.lineWidth
        };

        VkSampleMask sampleMask = desc.multisample.mask;

        VkPipelineMultisampleStateCreateInfo multiSampleInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = convertSampleCountIntoVulkan(desc.multisample.count),

            .sampleShadingEnable = desc.multisample.count > 1 ? VK_TRUE : VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = &sampleMask,

            .alphaToCoverageEnable = desc.multisample.alphaToCoverageEnabled ? VK_TRUE : VK_FALSE,
            .alphaToOneEnable = desc.multisample.alphaToOneEnabled ? VK_TRUE : VK_FALSE,
        };

        VkStencilOpState frontStencil{
            .failOp = convertStencilOpIntoVulkan(desc.stencil.stencilFront.failOp),
            .passOp = convertStencilOpIntoVulkan(desc.stencil.stencilFront.passOp),
            .depthFailOp = convertStencilOpIntoVulkan(desc.stencil.stencilFront.depthFailOp),

            .compareOp = convertCompareOpToVulkan(desc.stencil.stencilFront.compareOp),
            .compareMask = desc.stencil.stencilCompareMask,

            .writeMask = desc.stencil.stencilWriteMask,
            .reference = desc.stencil.stencilRefence
        };

        VkStencilOpState backStencil{
            .failOp = convertStencilOpIntoVulkan(desc.stencil.stencilBack.failOp),
            .passOp = convertStencilOpIntoVulkan(desc.stencil.stencilBack.passOp),
            .depthFailOp = convertStencilOpIntoVulkan(desc.stencil.stencilBack.depthFailOp),

            .compareOp = convertCompareOpToVulkan(desc.stencil.stencilBack.compareOp),
            .compareMask = desc.stencil.stencilCompareMask,
            
            .writeMask = desc.stencil.stencilWriteMask,
            .reference = desc.stencil.stencilRefence
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = desc.depth.depthTestEnabled ? VK_TRUE : VK_FALSE,
            .depthWriteEnable = desc.depth.depthWriteEnabled ? VK_TRUE : VK_FALSE,
            .depthCompareOp = convertCompareOpToVulkan(desc.depth.depthCompareOp),
            .depthBoundsTestEnable = desc.depth.depthBoundsTestEnabled ? VK_TRUE : VK_FALSE,
            .stencilTestEnable = desc.stencil.stencilTestEnabled ? VK_TRUE : VK_FALSE,
            .front = frontStencil,
            .back = backStencil,
            .minDepthBounds = desc.depth.minDepthBounds,
            .maxDepthBounds = desc.depth.maxDepthBounds
        };

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
        for (auto &&target : desc.fragment.targets) {
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
            .logicOpEnable = desc.fragment.logicOp != LogicOp::eCopy ? VK_TRUE : VK_FALSE,
            .logicOp = convertLogicOpIntoVulkan(desc.fragment.logicOp),
            .attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
            .pAttachments = colorBlendAttachments.data(),
            .blendConstants[0] = desc.fragment.blendConstant.r,
            .blendConstants[1] = desc.fragment.blendConstant.g,
            .blendConstants[2] = desc.fragment.blendConstant.b,
            .blendConstants[3] = desc.fragment.blendConstant.a
        };

        std::vector<VkDynamicState> dynamicStates = convertDynamicStatesIntoVulkan(desc.dynamicState);

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        std::vector<VkFormat> colorAttachmentFormats{};
        for (auto &&target : desc.fragment.targets) {
            colorAttachmentFormats.emplace_back(convertTextureFormatIntoVulkan(target.format));
        }

        VkPipelineRenderingCreateInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size()),
            .pColorAttachmentFormats = colorAttachmentFormats.data(),
            .depthAttachmentFormat = convertTextureFormatIntoVulkan(desc.depth.format),
            .stencilAttachmentFormat = convertTextureFormatIntoVulkan(desc.stencil.format)
        };

        VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &renderingInfo,

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

            .layout = dynamic_cast<VulkanPipelineLayout*>(desc.layout)->getNative(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0
        };

        VkPipeline pipeline;
        if (vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphic pipeline!");
        }

        return std::make_shared<VulkanRenderPipeline>(desc, this, pipeline);
    }

    VulkanRenderPipeline::~VulkanRenderPipeline() {
        vkDestroyPipeline(this->device->getNative(), this->pipeline, nullptr);
    }

    BindGroupLayout* VulkanRenderPipeline::getBindGroupLayout(Uint32 index) {
        return this->desc.layout->getDesc().bindGroupLayouts[index];
    }
}