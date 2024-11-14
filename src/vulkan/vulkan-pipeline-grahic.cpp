#include "vulkan-backend.hpp"

#include <array>
#include <utility>

namespace RHI {
    std::shared_ptr<RenderPipeline> VulkanDevice::createRenderPipeline(RenderPipelineDescriptor desc) {
        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageInfos{};

        shaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStageInfos[0].module = dynamic_cast<VulkanShaderModule*>(desc.vertex.module)->getNative();

        shaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageInfos[1].module = dynamic_cast<VulkanShaderModule*>(desc.fragment.module)->getNative();

        std::vector<VkVertexInputBindingDescription> vertexBindingInfoes{};
        std::vector<VkVertexInputAttributeDescription> vertexAttributeInfoes{};

        for (uint32_t i = 0; i < desc.vertex.buffers.size(); i++) {
            VertexBufferLayout vertexBufferLayout = desc.vertex.buffers[i];

            VkVertexInputBindingDescription vertexBindingInfo{};
            vertexBindingInfo.binding = i;
            vertexBindingInfo.stride = vertexBufferLayout.arrayStride;
            vertexBindingInfo.inputRate = convertVertexStepModeIntoVulkan(vertexBufferLayout.stepMode);

            vertexBindingInfoes.emplace_back(vertexBindingInfo);
            
            for (uint32_t j = 0; j < vertexBufferLayout.attributes.size(); j++) {
                VertexAttribute vertexAttribute = vertexBufferLayout.attributes[j];

                VkVertexInputAttributeDescription vertexAttributeInfo{};
                vertexAttributeInfo.binding = i;
                vertexAttributeInfo.location = j;
                vertexAttributeInfo.format = convertVertexFormatIntoVulkan(vertexAttribute.format);
                vertexAttributeInfo.offset = vertexAttribute.offset;

                vertexAttributeInfoes.emplace_back(vertexAttributeInfo);
            }
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingInfoes.size());
        vertexInputInfo.pVertexBindingDescriptions = vertexBindingInfoes.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeInfoes.size());
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeInfoes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = convertPrimitiveTopologyIntoVulkan(desc.primitive.topology);
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        std::vector<VkViewport> vulkanViewports{};
        std::vector<VkRect2D> vulkanScissors{};

        for (auto &&viewport : desc.viewportScissor.viewports) {
            vulkanViewports.emplace_back(convertViewportIntoVulkan(viewport));
        }

        for (auto &&scissor : desc.viewportScissor.scissors) {
            vulkanScissors.emplace_back(convertRect2DIntoVulkan(scissor));
        }

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = static_cast<uint32_t>(vulkanViewports.size());
        viewportInfo.pViewports = vulkanViewports.data();
        viewportInfo.scissorCount = static_cast<uint32_t>(vulkanScissors.size());
        viewportInfo.pScissors = vulkanScissors.data();

        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.frontFace = convertFrontFaceIntoVulkan(desc.rasterizationState.frontFace);
        rasterizationInfo.cullMode = convertCullModeIntoVulkan(desc.rasterizationState.cullMode);
        rasterizationInfo.polygonMode = convertPolygonModeIntoVulkan(desc.rasterizationState.polygonMode);
        rasterizationInfo.depthBiasEnable = desc.rasterizationState.depthBias.constant > 0.0f ? VK_TRUE : VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = desc.rasterizationState.depthBias.constant;
        rasterizationInfo.depthBiasClamp = desc.rasterizationState.depthBias.clamp;
        rasterizationInfo.depthBiasSlopeFactor = desc.rasterizationState.depthBias.slopeScale;
        rasterizationInfo.lineWidth = desc.rasterizationState.lineWidth;

        VkSampleMask sampleMask = desc.multisample.mask;

        VkPipelineMultisampleStateCreateInfo multiSampleInfo{};
        multiSampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSampleInfo.rasterizationSamples = convertSampleCountIntoVulkan(desc.multisample.count);
        multiSampleInfo.sampleShadingEnable = desc.multisample.count > 1 ? VK_TRUE : VK_FALSE;
        multiSampleInfo.minSampleShading = 1.0f;
        multiSampleInfo.pSampleMask = &sampleMask;
        multiSampleInfo.alphaToCoverageEnable = desc.multisample.alphaToCoverageEnabled ? VK_TRUE : VK_FALSE;
        multiSampleInfo.alphaToOneEnable = desc.multisample.alphaToOneEnabled ? VK_TRUE : VK_FALSE;

        VkStencilOpState frontStencil{};
        frontStencil.failOp = convertStencilOpIntoVulkan(desc.stencil.stencilFront.failOp);
        frontStencil.passOp = convertStencilOpIntoVulkan(desc.stencil.stencilFront.passOp);
        frontStencil.depthFailOp = convertStencilOpIntoVulkan(desc.stencil.stencilFront.depthFailOp);
        frontStencil.compareOp = convertCompareOpToVulkan(desc.stencil.stencilFront.compareOp);
        frontStencil.compareMask = desc.stencil.stencilCompareMask;
        frontStencil.writeMask = desc.stencil.stencilWriteMask;
        frontStencil.reference = desc.stencil.stencilRefence;

        VkStencilOpState backStencil{};
        backStencil.failOp = convertStencilOpIntoVulkan(desc.stencil.stencilBack.failOp);
        backStencil.passOp = convertStencilOpIntoVulkan(desc.stencil.stencilBack.passOp);
        backStencil.depthFailOp = convertStencilOpIntoVulkan(desc.stencil.stencilBack.depthFailOp);
        backStencil.compareOp = convertCompareOpToVulkan(desc.stencil.stencilBack.compareOp);
        backStencil.compareMask = desc.stencil.stencilCompareMask;
        backStencil.writeMask = desc.stencil.stencilWriteMask;
        backStencil.reference = desc.stencil.stencilRefence;

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.depthTestEnable = desc.depth.depthTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencilInfo.depthWriteEnable = desc.depth.depthWriteEnabled ? VK_TRUE : VK_FALSE;
        depthStencilInfo.depthCompareOp = convertCompareOpToVulkan(desc.depth.depthCompareOp);
        depthStencilInfo.depthBoundsTestEnable = desc.depth.depthBoundsTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencilInfo.stencilTestEnable = desc.stencil.stencilTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencilInfo.front = frontStencil;
        depthStencilInfo.back = backStencil;
        depthStencilInfo.minDepthBounds = desc.depth.minDepthBounds;
        depthStencilInfo.maxDepthBounds = desc.depth.maxDepthBounds;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
        for (auto &&target : desc.fragment.targets) {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};

            colorBlendAttachment.blendEnable = target.blend.blendEnabled ? VK_TRUE : VK_FALSE;
            colorBlendAttachment.colorWriteMask = convertColorComponentIntoVulkan(target.colorWriteMask);

            colorBlendAttachment.srcColorBlendFactor = convertBlendFactorIntoVulkan(target.blend.color.srcFactor);
            colorBlendAttachment.dstColorBlendFactor = convertBlendFactorIntoVulkan(target.blend.color.dstFactor);
            colorBlendAttachment.colorBlendOp = convertBlendOperationIntoVulkan(target.blend.color.operation);

            colorBlendAttachment.srcAlphaBlendFactor = convertBlendFactorIntoVulkan(target.blend.alpha.srcFactor);
            colorBlendAttachment.dstAlphaBlendFactor = convertBlendFactorIntoVulkan(target.blend.alpha.dstFactor);
            colorBlendAttachment.alphaBlendOp = convertBlendOperationIntoVulkan(target.blend.alpha.operation);

            colorBlendAttachments.emplace_back(colorBlendAttachment);
        }

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = desc.fragment.logicOp != LogicOp::eCopy ? VK_TRUE : VK_FALSE;
        colorBlendInfo.logicOp = convertLogicOpIntoVulkan(desc.fragment.logicOp);
        colorBlendInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
        colorBlendInfo.pAttachments = colorBlendAttachments.data();
        colorBlendInfo.blendConstants[0] = desc.fragment.blendConstant.r;
        colorBlendInfo.blendConstants[1] = desc.fragment.blendConstant.g;
        colorBlendInfo.blendConstants[2] = desc.fragment.blendConstant.b;
        colorBlendInfo.blendConstants[3] = desc.fragment.blendConstant.a;

        std::vector<VkDynamicState> dynamicStates = convertDynamicStatesIntoVulkan(desc.dynamicState);

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        std::vector<VkFormat> colorAttachmentFormats{};
        for (auto &&target : desc.fragment.targets) {
            colorAttachmentFormats.emplace_back(convertTextureFormatIntoVulkan(target.format));
        }

        VkPipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size());
        renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
        renderingInfo.depthAttachmentFormat = convertTextureFormatIntoVulkan(desc.depth.format);
        renderingInfo.stencilAttachmentFormat = convertTextureFormatIntoVulkan(desc.stencil.format);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &renderingInfo;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
        pipelineInfo.pStages = shaderStageInfos.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multiSampleInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = dynamic_cast<VulkanPipelineLayout*>(desc.layout)->getNative();
        pipelineInfo.renderPass = VK_NULL_HANDLE;
        pipelineInfo.subpass = 0;

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