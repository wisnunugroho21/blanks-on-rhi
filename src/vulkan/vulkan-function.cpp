#include "vulkan-backend.hpp"

#include <utility>

namespace RHI {
    VkRect2D convertRect2DIntoVulkan(Rect2D rect) {
        VkRect2D vulkanRect{
            .offset.x = rect.x,
            .offset.y = rect.y,
            .extent.width = rect.width,
            .extent.height = rect.height
        };

        return vulkanRect;
    }

    VkBufferUsageFlags convertBufferUsageIntoVulkan(BufferUsageFlags usage) {
        VkBufferUsageFlags vulkanBufferFlags = 0;

        if (usage & std::to_underlying(BufferUsage::eCopySrc)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        } 
        
        if (usage & std::to_underlying(BufferUsage::eCopyDst)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        } 
        
        if (usage & std::to_underlying(BufferUsage::eIndex)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        } 
        
        if (usage & std::to_underlying(BufferUsage::eVertex)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        } 
        
        if (usage & std::to_underlying(BufferUsage::eUniform)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        } 
        
        if (usage & std::to_underlying(BufferUsage::eStorage)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        } 
        
        if (usage & std::to_underlying(BufferUsage::eIndirect)) {
            vulkanBufferFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return vulkanBufferFlags;
    }

    VmaMemoryUsage convertBufferLocationIntoVulkan(BufferLocation location) {
        switch (location) {
            case BufferLocation::eDeviceLocal:
                return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            
            case BufferLocation::eHost:
                return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            
            default:
                return VMA_MEMORY_USAGE_AUTO;
        }
    }

    VmaAllocationCreateFlags convertToAllocationFlag(BufferUsageFlags usage, BufferLocation location) {
        if (usage & std::to_underlying(BufferUsage::eCopySrc) && location == BufferLocation::eHost) {
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        } 
        
        else if (usage & std::to_underlying(BufferUsage::eCopyDst) && location == BufferLocation::eHost) {
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        } 
        
        else if (location == BufferLocation::eDeviceLocal) {
            return VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        }

        return VmaAllocationCreateFlags{};
    }

    VkFormat convertTextureFormatIntoVulkan(TextureFormat format) {
        switch (format) {
            // 8-bit formats
            case TextureFormat::eR8Unorm:
                return VK_FORMAT_R8_UNORM;
            case TextureFormat::eR8Snorm:
                return VK_FORMAT_R8_SNORM;
            case TextureFormat::eR8Uint:
                return VK_FORMAT_R8_UINT;
            case TextureFormat::eR8Sint:
                return VK_FORMAT_R8_SINT;

            // 16-bit formats
            case TextureFormat::eR16Uint:
                return VK_FORMAT_R16_UINT;
            case TextureFormat::eR16Sint:
                return VK_FORMAT_R16_SINT;
            case TextureFormat::eR16Float:
                return VK_FORMAT_R16_SFLOAT;
            case TextureFormat::eRG8Unorm:
                return VK_FORMAT_R8G8_UNORM;
            case TextureFormat::eRG8Snorm:
                return VK_FORMAT_R8G8_SNORM;
            case TextureFormat::eRG8Uint:
                return VK_FORMAT_R8G8_UINT;
            case TextureFormat::eRG8Sint:
                return VK_FORMAT_R8G8_SINT;

            // 32-bit formats
            case TextureFormat::eR32Uint:
                return VK_FORMAT_R32_UINT;
            case TextureFormat::eR32Sint:
                return VK_FORMAT_R32_SINT;
            case TextureFormat::eR32Float:
                return VK_FORMAT_R32_SFLOAT;
            case TextureFormat::eRG16Uint:
                return VK_FORMAT_R16G16_UINT;
            case TextureFormat::eRG16Sint:
                return VK_FORMAT_R16G16_SINT;
            case TextureFormat::eRG16Float:
                return VK_FORMAT_R16G16_SFLOAT;
            case TextureFormat::eRGBA8Unorm:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::eRGBA8UnormSrgb:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::eRGBA8Snorm:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case TextureFormat::eRGBA8Uint:
                return VK_FORMAT_R8G8B8A8_UINT;
            case TextureFormat::eRGBA8Sint:
                return VK_FORMAT_R8G8B8A8_SINT;
            case TextureFormat::eBGRA8Unorm:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::eBGRA8UnormSrgb:
                return VK_FORMAT_B8G8R8A8_SRGB;

            // Packed 32-bit formats
            case TextureFormat::eRGB9E5Ufloat:
                return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
            case TextureFormat::eRGB10A2Uint:
                return VK_FORMAT_A2R10G10B10_UINT_PACK32;
            case TextureFormat::eRGB10A2Unorm:
                return VK_FORMAT_A2R10G10B10_UNORM_PACK32;

            // 64-bit formats
            case TextureFormat::eRG32Uint:
                return VK_FORMAT_R32G32_UINT;
            case TextureFormat::eRG32Sint:
                return VK_FORMAT_R32G32_SINT;
            case TextureFormat::eRG32Float:
                return VK_FORMAT_R32G32_SFLOAT;
            case TextureFormat::eRGBA16Uint:
                return VK_FORMAT_R16G16B16A16_UINT;
            case TextureFormat::eRGBA16Sint:
                return VK_FORMAT_R16G16B16A16_SINT;
            case TextureFormat::eRGBA16Float:
                return VK_FORMAT_R16G16B16A16_SFLOAT;

            // 128-bit formats
            case TextureFormat::eRGBA32Uint:
                return VK_FORMAT_R32G32B32A32_UINT;
            case TextureFormat::eRGBA32Sint:
                return VK_FORMAT_R32G32B32A32_SINT;
            case TextureFormat::eRGBA32Float:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
                
            // Depth/stencil formats
            case TextureFormat::eS8Uint:
                return VK_FORMAT_S8_UINT;
            case TextureFormat::eD16Unorm:
                return VK_FORMAT_D16_UNORM;
            case TextureFormat::eD24Plus:
                return VK_FORMAT_D32_SFLOAT;
            case TextureFormat::eD24PlusS8Uint:
                return VK_FORMAT_D24_UNORM_S8_UINT;
            case TextureFormat::eD32Sfloat:
                return VK_FORMAT_D32_SFLOAT;
            
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    VkImageType convertDimensionIntoVulkan(TextureDimension dimension) {
        switch (dimension) {
            case TextureDimension::e1D : return VK_IMAGE_TYPE_1D;
            case TextureDimension::e2D : return VK_IMAGE_TYPE_2D;
            case TextureDimension::e3D : return VK_IMAGE_TYPE_3D;

            default: return VK_IMAGE_TYPE_2D;
        }
    }

    VkSampleCountFlagBits convertSampleCountIntoVulkan(Uint32 sampleCount) {
        if (sampleCount == 1) {
            return VK_SAMPLE_COUNT_1_BIT;
        } else if (sampleCount == 2 && sampleCount == 3) {
            return VK_SAMPLE_COUNT_2_BIT;
        } else if (sampleCount >= 4 && sampleCount <= 7) {
            return VK_SAMPLE_COUNT_4_BIT;
        } else if (sampleCount >= 8 && sampleCount <= 15) {
            return VK_SAMPLE_COUNT_8_BIT;
        } else if (sampleCount >= 16 && sampleCount <= 31) {
            return VK_SAMPLE_COUNT_16_BIT;
        } else if (sampleCount >= 32) {
            return VK_SAMPLE_COUNT_32_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkImageUsageFlags convertImageUsageIntoVulkan(TextureUsageFlags usage) {
        VkImageUsageFlags vulkanImageFlags = 0;

        if (usage & std::to_underlying(TextureUsage::eCopySrc)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eCopyDst)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eTextureBinding)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eStorageBinding)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eColorAttachment)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        } 
        
        if (usage & std::to_underlying(TextureUsage::eDepthStencilAttachment)) {
            vulkanImageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        return vulkanImageFlags;
    }

    VkImageLayout convertTextureStateIntoVulkan(TextureState state) {
        switch (state) {
            case TextureState::eUndefined : return VK_IMAGE_LAYOUT_UNDEFINED;
            case TextureState::eStorageBinding : return VK_IMAGE_LAYOUT_GENERAL;
            case TextureState::eColorAttachment : return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case TextureState::eDepthStencilAttachment : return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case TextureState::eColorTextureBinding : return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case TextureState::eDepthStencilTextureBinding : return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case TextureState::eCopySrc : return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case TextureState::eCopyDst : return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case TextureState::ePresent : return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            default : return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    VkImageViewType convertTextureViewDimensionIntoVulkan(TextureViewDimension viewDimension) {
        switch (viewDimension) {
            case TextureViewDimension::e1D : return VK_IMAGE_VIEW_TYPE_1D;
            case TextureViewDimension::e2D : return VK_IMAGE_VIEW_TYPE_2D;
            case TextureViewDimension::e2DArray : return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case TextureViewDimension::eCube : return VK_IMAGE_VIEW_TYPE_CUBE;
            case TextureViewDimension::eCubeArray : return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            case TextureViewDimension::e3D : return VK_IMAGE_VIEW_TYPE_3D;
        
            default: return VK_IMAGE_VIEW_TYPE_2D;
        }
    }

    VkImageAspectFlags convertAspectIntoVulkan(TextureAspect aspect) {
        switch (aspect) {
            case TextureAspect::eColor : return VK_IMAGE_ASPECT_COLOR_BIT;
            case TextureAspect::eDepth : return VK_IMAGE_ASPECT_DEPTH_BIT;
            case TextureAspect::eStencil : return VK_IMAGE_ASPECT_STENCIL_BIT;
            case TextureAspect::eDepthStencil : return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            
            default: return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    VkSamplerAddressMode convertAddressModeToVulkan(AddressMode addressMode) {
        switch (addressMode) {
            case AddressMode::eRepeat : return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case AddressMode::eMirrorRepeat : return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case AddressMode::eClampToEdge : return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case AddressMode::eClampToBorder : return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            
            default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    VkFilter convertFilterToVulkan(FilterMode filterMode) {
        switch (filterMode) {
            case FilterMode::eNearest : return VK_FILTER_NEAREST;
            case FilterMode::eLinear : return VK_FILTER_LINEAR;
            
            default: return VK_FILTER_NEAREST;
        }
    }

    VkSamplerMipmapMode convertMipmapFilterToVulkan(MipmapFilterMode mipmapFilterMode) {
        switch (mipmapFilterMode) {
            case MipmapFilterMode::eNearest : return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case MipmapFilterMode::eLinear : return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            
            default: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }
    }

    VkCompareOp convertCompareOpToVulkan(CompareFunction compareFunc) {
        switch (compareFunc) {
            case CompareFunction::eNever : return VK_COMPARE_OP_NEVER;
            case CompareFunction::eEqual : return VK_COMPARE_OP_EQUAL;
            case CompareFunction::eLess : return VK_COMPARE_OP_LESS;
            case CompareFunction::eLessEqual : return VK_COMPARE_OP_LESS_OR_EQUAL;
            case CompareFunction::eGreater : return VK_COMPARE_OP_GREATER;
            case CompareFunction::eGreaterEqual : return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case CompareFunction::eNotEqual : return VK_COMPARE_OP_NOT_EQUAL;
            case CompareFunction::eAlways : return VK_COMPARE_OP_ALWAYS;
            
            default: return VK_COMPARE_OP_NEVER;
        }
    }

    VkBorderColor convertBorderColorToVulkan(BorderColor borderColor) {
        switch (borderColor) {
            case BorderColor::eFloatTransparentBlack : return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            case BorderColor::eIntTransparentBlack : return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            case BorderColor::eFloatOpaqueBlack : return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            case BorderColor::eIntOpaqueBlack : return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            case BorderColor::eFloatOpaqueWhite : return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            case BorderColor::eIntOpaqueWhite : return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
            
            default: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        }
    }

    VkDescriptorType convertBindTypeIntoVulkan(BindingType type) {
        switch (type) {
            case BindingType::eUniformBuffer : return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            case BindingType::eStorageBuffer : return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
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

    VkPipelineStageFlags convertPipelineStageIntoVulkan(PipelineStage stage) {
        switch(stage) {
            case PipelineStage::eCompute: return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            case PipelineStage::eVertex: return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            case PipelineStage::eFragment: return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            case PipelineStage::eTessellCtrl: return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
            case PipelineStage::eTessellEval: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
            case PipelineStage::eTask: return VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
            case PipelineStage::eMesh: return VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
            case PipelineStage::eAttachmentOutput: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            case PipelineStage::eEarlyFragmentTest: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case PipelineStage::eLateFragmentTest: return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            case PipelineStage::eTransfer: return VK_PIPELINE_STAGE_TRANSFER_BIT;
            case PipelineStage::ePresent: return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }

        return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }

    VkViewport convertViewportIntoVulkan(Viewport viewport) {
        VkViewport vulkanViewPort{
            .x = viewport.x,
            .y = viewport.y,
            .width = viewport.width,
            .height = viewport.height,
            .maxDepth = viewport.maxDepth,
            .minDepth = viewport.minDepth
        };

        return vulkanViewPort;
    }

    VkVertexInputRate convertVertexStepModeIntoVulkan(VertexStepMode stepMode) {
        switch (stepMode) {
            case VertexStepMode::eVertex : return VK_VERTEX_INPUT_RATE_VERTEX;
            case VertexStepMode::eInstance : return VK_VERTEX_INPUT_RATE_INSTANCE;
            
            default: return VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    VkFormat convertVertexFormatIntoVulkan(VertexFormat format) {
        switch (format) {
            case VertexFormat::eUint8x2 :
                return VK_FORMAT_R8G8_UINT;
            case VertexFormat::eUint8x4 :
                return VK_FORMAT_R8G8B8A8_UINT;
            case VertexFormat::eSint8x2 :
                return VK_FORMAT_R8G8_SINT;
            case VertexFormat::eSint8x4 :
                return VK_FORMAT_R8G8B8A8_SINT;
            case VertexFormat::eUnorm8x2 :
                return VK_FORMAT_R8G8_UNORM;
            case VertexFormat::eUnorm8x4 :
                return VK_FORMAT_R8G8B8A8_UNORM;
            case VertexFormat::eSnorm8x2 :
                return VK_FORMAT_R8G8_SNORM;
            case VertexFormat::eSnorm8x4 :
                return VK_FORMAT_R8G8B8A8_SNORM;

            case VertexFormat::eUint16x2 :
                return VK_FORMAT_R16G16_UINT;
            case VertexFormat::eUint16x4 :
                return VK_FORMAT_R16G16B16A16_UINT;
            case VertexFormat::eSint16x2 :
                return VK_FORMAT_R16G16_SINT;
            case VertexFormat::eSint16x4 :
                return VK_FORMAT_R16G16B16A16_SINT;
            case VertexFormat::eUnorm16x2 :
                return VK_FORMAT_R16G16_UNORM;
            case VertexFormat::eUnorm16x4 :
                return VK_FORMAT_R16G16B16A16_UNORM;
            case VertexFormat::eSnorm16x2 :
                return VK_FORMAT_R16G16_SNORM;
            case VertexFormat::eSnorm16x4 :
                return VK_FORMAT_R16G16B16A16_SNORM;
            case VertexFormat::eFloat16x2 :
                return VK_FORMAT_R16G16_SFLOAT;
            case VertexFormat::eFloat16x4 :
                return VK_FORMAT_R16G16B16A16_SFLOAT;

            case VertexFormat::eFloat32 :
                return VK_FORMAT_R32_SFLOAT;
            case VertexFormat::eFloat32x2 :
                return VK_FORMAT_R32G32_SFLOAT;
            case VertexFormat::eFloat32x4 :
                return VK_FORMAT_R32G32B32A32_SFLOAT;

            case VertexFormat::eUint32 :
                return VK_FORMAT_R32_UINT;
            case VertexFormat::eUint32x2 :
                return VK_FORMAT_R32G32_UINT;
            case VertexFormat::eUint32x4 :
                return VK_FORMAT_R32G32B32A32_UINT;

            case VertexFormat::eSint32 :
                return VK_FORMAT_R32_SINT;
            case VertexFormat::eSint32x2 :
                return VK_FORMAT_R32G32_SINT;
            case VertexFormat::eSint32x4 :
                return VK_FORMAT_R32G32B32A32_SINT;
            
            default:
                break;
        }
    }

    VkPrimitiveTopology convertPrimitiveTopologyIntoVulkan(PrimitiveTopology topology) {
        switch (topology) {
            case PrimitiveTopology::ePointList : return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case PrimitiveTopology::eLineList : return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case PrimitiveTopology::eLineStrip : return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case PrimitiveTopology::eTriangleList : return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PrimitiveTopology::eTriangleStrip : return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            
            default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        }
    }

    VkFrontFace convertFrontFaceIntoVulkan(FrontFace frontFace) {
        switch(frontFace) {
            case FrontFace::eCCW: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            case FrontFace::eCW: return VK_FRONT_FACE_CLOCKWISE;

            default: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
    }

    VkCullModeFlagBits convertCullModeIntoVulkan(CullMode mode) {
        switch (mode) {
            case CullMode::eNone : return VK_CULL_MODE_NONE;
            case CullMode::eFront : return VK_CULL_MODE_FRONT_BIT;
            case CullMode::eBack : return VK_CULL_MODE_BACK_BIT;
            case CullMode::eAll : return VK_CULL_MODE_FRONT_AND_BACK;
            
            default: return VK_CULL_MODE_NONE;
        }
    }

    VkPolygonMode convertPolygonModeIntoVulkan(PolygonMode mode) {
        switch (mode) {
            case PolygonMode::eFill : return VK_POLYGON_MODE_FILL;
            case PolygonMode::eLine : return VK_POLYGON_MODE_LINE;
            case PolygonMode::ePoint : return VK_POLYGON_MODE_POINT;
        
            default: return VK_POLYGON_MODE_FILL;
        }
    }

    VkStencilOp convertStencilOpIntoVulkan(StencilOperation stencilOp) {
        switch (stencilOp) {
            case StencilOperation::eKeep : return VK_STENCIL_OP_KEEP;
            case StencilOperation::eZero : return VK_STENCIL_OP_ZERO;
            case StencilOperation::eReplace : return VK_STENCIL_OP_REPLACE;
            case StencilOperation::eInvert : return VK_STENCIL_OP_INVERT;
            case StencilOperation::eIncrementClamp : return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            case StencilOperation::eDecrementClamp : return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            
            default: return VK_STENCIL_OP_KEEP;
        }
    }

    VkLogicOp convertLogicOpIntoVulkan(LogicOp logicOp) {
        switch (logicOp) {
            case LogicOp::eCopy : return VK_LOGIC_OP_COPY;
            case LogicOp::eKeep : return VK_LOGIC_OP_NO_OP;
            case LogicOp::eClear : return VK_LOGIC_OP_CLEAR;
            case LogicOp::eAnd : return VK_LOGIC_OP_AND;
            case LogicOp::eAndReverse : return VK_LOGIC_OP_AND_REVERSE;
            case LogicOp::eAndInverted : return VK_LOGIC_OP_AND_INVERTED;
            case LogicOp::eXor : return VK_LOGIC_OP_XOR;
            case LogicOp::eOr : return VK_LOGIC_OP_OR;
            case LogicOp::eNor : return VK_LOGIC_OP_NOR;
            case LogicOp::eEquivalent : return VK_LOGIC_OP_EQUIVALENT;
            case LogicOp::eInvert : return VK_LOGIC_OP_INVERT;
            case LogicOp::eOrReverse : return VK_LOGIC_OP_OR_REVERSE;
            case LogicOp::eOrInverted : return VK_LOGIC_OP_OR_INVERTED;
            case LogicOp::eCopyInverted : return VK_LOGIC_OP_COPY_INVERTED;
            case LogicOp::eNand : return VK_LOGIC_OP_NAND;
            case LogicOp::eSet : return VK_LOGIC_OP_SET;
            
            default: return VK_LOGIC_OP_COPY;
        }
    }

    VkBlendOp convertBlendOperationIntoVulkan(BlendOperation blendOp) {
        switch (blendOp ) {
            case BlendOperation::eAdd : return VK_BLEND_OP_ADD;
            case BlendOperation::eSubtract : return VK_BLEND_OP_SUBTRACT;
            case BlendOperation::eReverseSubtract : return VK_BLEND_OP_REVERSE_SUBTRACT;
            case BlendOperation::eMin : return VK_BLEND_OP_MIN;
            case BlendOperation::eMax : return VK_BLEND_OP_MAX;
            
            default: return VK_BLEND_OP_ADD;
        }
    }

    VkBlendFactor convertBlendFactorIntoVulkan(BlendFactor blendFactor) {
        switch (blendFactor) {
            case BlendFactor::eZero : return VK_BLEND_FACTOR_ZERO;
            case BlendFactor::eOne : return VK_BLEND_FACTOR_ONE;
            case BlendFactor::eSrc : return VK_BLEND_FACTOR_SRC_COLOR;
            case BlendFactor::eOneMinusSrc : return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            case BlendFactor::eDst : return VK_BLEND_FACTOR_DST_COLOR;
            case BlendFactor::eOneMinusDst : return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            case BlendFactor::eSrcAlpha : return VK_BLEND_FACTOR_SRC_ALPHA;
            case BlendFactor::eOneMinusSrcAlpha : return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            case BlendFactor::eDstAlpha : return VK_BLEND_FACTOR_DST_ALPHA;
            case BlendFactor::eOneMinusDstAlpha : return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            case BlendFactor::eConstant : return VK_BLEND_FACTOR_CONSTANT_COLOR;
            case BlendFactor::eOneMinusConstant : return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
            case BlendFactor::eSrcAlphaSaturated : return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
            case BlendFactor::eSrc1 : return VK_BLEND_FACTOR_SRC1_COLOR;
            case BlendFactor::eOneMinusSrc1 : return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
            case BlendFactor::eSrc1Alpha : return VK_BLEND_FACTOR_SRC1_ALPHA;
            case BlendFactor::eOneMinusSrc1Alpha : return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
            
            default:
                break;
        }
    }

    VkColorComponentFlags convertColorComponentIntoVulkan(ColorWriteFlags colorWrite) {
        VkColorComponentFlags colorComponentFlags = 0;

        if (colorWrite & std::to_underlying(ColorWrite::eRed)) {
            colorComponentFlags |= VK_COLOR_COMPONENT_R_BIT;
        }

        if (colorWrite & std::to_underlying(ColorWrite::eGreen)) {
            colorComponentFlags |= VK_COLOR_COMPONENT_G_BIT;
        }

        if (colorWrite & std::to_underlying(ColorWrite::eBlue)) {
            colorComponentFlags |= VK_COLOR_COMPONENT_B_BIT;
        }

        if (colorWrite & std::to_underlying(ColorWrite::eAlpha)) {
            colorComponentFlags |= VK_COLOR_COMPONENT_A_BIT;
        }

        return colorComponentFlags;
    }

    std::vector<VkDynamicState> convertDynamicStatesIntoVulkan(DynamicStateEnabledState enabledState) {
        std::vector<VkDynamicState> dynamicStates{};

        if (enabledState.viewport) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
        }

        if (enabledState.scissorRect) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
        }

        if (enabledState.lineWidth) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
        }

        if (enabledState.depthBias) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
        }

        if (enabledState.blendConstants) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
        }

        if (enabledState.depthBounds) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
        }

        if (enabledState.stencilCompareMask) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
        }

        if (enabledState.stencilWriteMask) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
        }

        if (enabledState.stencilReference) {
            dynamicStates.emplace_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
        }

        return dynamicStates;
    }

    VkResolveModeFlagBits convertResolveModeIntoVulkan(ResolveMode resolveMode) {
        switch (resolveMode) {
            case ResolveMode::eAverage: return VK_RESOLVE_MODE_AVERAGE_BIT;
            case ResolveMode::eMax: return VK_RESOLVE_MODE_MAX_BIT;
            case ResolveMode::eMin: return VK_RESOLVE_MODE_MIN_BIT;

            default: return VK_RESOLVE_MODE_AVERAGE_BIT;
        }
    }

    VkAttachmentLoadOp convertLoadOpIntoVulkan(LoadOp loadOp) {
        switch (loadOp) {
            case LoadOp::eClear : return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case LoadOp::eLoad : return VK_ATTACHMENT_LOAD_OP_LOAD;
            
            default: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
    }

    VkAttachmentStoreOp convertStoreOpIntoVulkan(StoreOp storeOp) {
        switch (storeOp) {
            case StoreOp::eStore : return VK_ATTACHMENT_STORE_OP_STORE;
            case StoreOp::eDiscard : return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            
            default: return VK_ATTACHMENT_STORE_OP_STORE;
        }
    }

    VkIndexType convertIndexFormatIntoVulkan(IndexFormat format) {
        switch (format) {
            case IndexFormat::eUint16 : return VK_INDEX_TYPE_UINT16;
            case IndexFormat::eUint32 : return VK_INDEX_TYPE_UINT32;
            
            default: return VK_INDEX_TYPE_UINT32;
        }
    }
}