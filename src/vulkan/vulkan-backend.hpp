#pragma once

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include "../rhi.hpp"

namespace RHI {
    class VulkanDevice : public Device {
    public:
        VulkanDevice(
            DeviceDescriptor desc,
            VkInstance i,
            VkPhysicalDevice pd, 
            VkDevice d,
            VkDebugUtilsMessengerEXT dm,
            VkPhysicalDeviceProperties dp,
            VmaAllocator vma,
            std::map<QueueType, std::vector<std::shared_ptr<Queue>>> q,
            std::map<QueueType, VkCommandPool> c,
            VkDescriptorPool dsp            
        )
        : Device(desc),
          instance{i},
          physicalDevice{pd}, 
          device{d},
          debugMessenger{dm},
          deviceProperties{dp},
          memoryAllocator{vma},
          queues{q},
          commandPools{c},
          descriptorPool{dsp}
        {

        }
        
        VkDevice getNative() { return this->device; }
        VmaAllocator getMemoryAllocator() { return this->memoryAllocator; }
        VkDescriptorPool getDescriptorPool() { return this->descriptorPool; }

        std::shared_ptr<Buffer> createBuffer(BufferDescriptor descriptor) override;
        std::shared_ptr<Texture> createTexture(TextureDescriptor descriptor) override;
        std::shared_ptr<Sampler> createSampler(SamplerDescriptor desc) override;
        std::shared_ptr<RenderGraph> createRenderGraph(RenderGraphDescriptor desc) override;
        
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDeviceProperties deviceProperties;
        VmaAllocator memoryAllocator;

        std::map<QueueType, std::vector<std::shared_ptr<Queue>>> queues;
        std::map<QueueType, VkCommandPool> commandPools;
        VkDescriptorPool descriptorPool;
    };

    class VulkanBuffer : public Buffer {
    public:
        VulkanBuffer(
            BufferDescriptor desc,
            VulkanDevice* d,
            VkBuffer b,
            VmaAllocation ma
        )
        : desc{desc},
          device{d}, 
          buffer{b},
          memoryAllocation{ma}
        {
            this->mapState = BufferMapState::eUnmapped;
        }

        ~VulkanBuffer();

        BufferDescriptor getDesc() override { return this->desc; }

        void* getCurrentMapped() override { return this->mapped; }
		BufferMapState getMapState() override { return this->mapState; }

        void insertData(void* pointerData, Uint64 size = ULLONG_MAX, Uint64 offset = 0) override;
        void takeData(void* pointerData, Uint64 size = ULLONG_MAX, Uint64 offset = 0) override;

        void* map() override;
        void unmap() override;

        void flush(Uint64 size = ULLONG_MAX, Uint64 offset = 0) override;
        void invalidate(Uint64 size = ULLONG_MAX, Uint64 offset = 0) override;

        VkBuffer getNative() { return this->buffer; }
        VmaAllocation getMemoryAllocation() { return this->memoryAllocation; }

    protected:
        BufferDescriptor desc;

        void* mapped;
        BufferMapState mapState;

    private:
        VulkanDevice* device;

        VkBuffer buffer;
        VmaAllocation memoryAllocation;
    };

    class VulkanTexture : public Texture {
    public:
        VulkanTexture(
            TextureDescriptor desc,
            VulkanDevice* d,
            VkImage i,
            VmaAllocation ma
        )
        : desc{desc},
          device{d},
          image{i},
          memoryAllocation{ma}
        {

        }

        ~VulkanTexture();

        TextureDescriptor getDesc() override { return this->desc; }

        std::shared_ptr<TextureView> createView(TextureViewDescriptor descriptor) override;

        void setState(TextureState state) { this->state = state; }

        VkImage getNative() { return this->image; }
        VmaAllocation getMemoryAllocation() { return this->memoryAllocation; }

    protected:
        TextureDescriptor desc;
        TextureState state;

    private:
        VulkanDevice* device;

        VkImage image;
        VmaAllocation memoryAllocation;
    };

    class VulkanTextureView : public TextureView {
    public:
        VulkanTextureView(
            TextureViewDescriptor desc,
            Texture* t,
            VulkanDevice* d,
            VkImageView iv
        )
        : desc{desc},
          texture{t},
          device{d},
          imageView{iv}
        {

        }

        ~VulkanTextureView();

        TextureViewDescriptor getDesc() override { return this->desc; }

        Texture* getTexture() override { return this->texture; }

        VkImageView getNative() { return this->imageView; }

    protected:
        TextureViewDescriptor desc;
        Texture* texture;

    private:
        VulkanDevice* device;
        VkImageView imageView;
    };

    class VulkanSampler : public Sampler {
    public:
        VulkanSampler(
            SamplerDescriptor desc,
            VulkanDevice* d,
            VkSampler s
        )
        : desc{desc},
          device{d},
          sampler{s}
        {

        }

        ~VulkanSampler();

        SamplerDescriptor getDesc() override { return this->desc; }

        bool isComparison() override { 
            return this->desc.compare != CompareFunction::eNever; 
        }

        bool isFiltering() override { 
            return this->desc.magFilter == FilterMode::eLinear || this->desc.minFilter == FilterMode::eLinear;
        }

        VkSampler getNative() { return this->sampler; }

    protected:
        SamplerDescriptor desc;

    private:
        VulkanDevice* device;
        VkSampler sampler;
    };

    struct VulkanBindGroupLayoutNode {
        VkDescriptorSetLayout bindGroupLayout;
    };

    struct VulkanPipelineNode {
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;
        std::vector<VulkanBindGroupLayoutNode> bindGroupNodes;
    };

    struct VulkanRenderPassNode {
        VkRenderPass renderPass;
        std::vector<VulkanPipelineNode> pipelineNodes;
    };

    class VulkanRenderGraph : public RenderGraph {
    public:
        VulkanRenderGraph(
            RenderGraphDescriptor desc,
            std::vector<VulkanRenderPassNode> renderPassNodes
        )
        : desc{desc},
          renderPassNodes{renderPassNodes}
        {

        }

    protected:
        RenderGraphDescriptor desc;

    private:
        VulkanDevice* device;
        std::vector<VulkanRenderPassNode> renderPassNodes;
    };

    class VulkanShaderModule : public ShaderModule {
    public:
        VulkanShaderModule(
            ShaderModuleDescriptor desc,
            VulkanDevice* d,
            VkShaderModule sm
        )
        : desc{desc},
          device{d},
          shaderModule{sm}
        {

        }

        ~VulkanShaderModule();

        ShaderModuleDescriptor getDesc() override { return this->desc; }

        VkShaderModule getNative() { return this->shaderModule; }

    protected:
        ShaderModuleDescriptor desc;

    private:
        VulkanDevice* device;
        VkShaderModule shaderModule;
    };

    class VulkanFactory {
        static std::shared_ptr<Device> createDevice(DeviceDescriptor desc);
    };

    VkRect2D convertRect2DIntoVulkan(Rect2D rect);

    VkBufferUsageFlags convertBufferUsageIntoVulkan(BufferUsageFlags usage);

    VmaMemoryUsage convertBufferLocationIntoVulkan(BufferLocation location);

    VmaAllocationCreateFlags convertToAllocationFlag(BufferUsageFlags usage, BufferLocation location);

    VkFormat convertTextureFormatIntoVulkan(TextureFormat format);

    VkImageType convertDimensionIntoVulkan(TextureDimension dimension);

    VkSampleCountFlagBits convertSampleCountIntoVulkan(Uint32 sampleCount);

    VkImageUsageFlags convertImageUsageIntoVulkan(TextureUsageFlags usage);

    VkImageLayout convertTextureStateIntoVulkan(TextureState state);

    VkImageViewType convertTextureViewDimensionIntoVulkan(TextureViewDimension viewDimension);

    VkImageAspectFlags convertAspectIntoVulkan(TextureAspect aspect);

    VkSamplerAddressMode convertAddressModeToVulkan(AddressMode addressMode);

    VkFilter convertFilterToVulkan(FilterMode filterMode);

    VkSamplerMipmapMode convertMipmapFilterToVulkan(MipmapFilterMode mipmapFilterMode);

    VkCompareOp convertCompareOpToVulkan(CompareFunction compareFunc);

    VkBorderColor convertBorderColorToVulkan(BorderColor borderColor);

    VkDescriptorType convertBindTypeIntoVulkan(BindingType type);

    VkShaderStageFlags convertShaderStageIntoVulkan(ShaderStageFlags stage);

    VkViewport convertViewportIntoVulkan(Viewport viewport);

    VkVertexInputRate convertVertexStepModeIntoVulkan(VertexStepMode stepMode);

    VkFormat convertVertexFormatIntoVulkan(VertexFormat format);

    VkPrimitiveTopology convertPrimitiveTopologyIntoVulkan(PrimitiveTopology topology);

    VkFrontFace convertFrontFaceIntoVulkan(FrontFace frontFace);

    VkCullModeFlagBits convertCullModeIntoVulkan(CullMode mode);

    VkPolygonMode convertPolygonModeIntoVulkan(PolygonMode mode);

    VkVertexInputRate convertVertexStepModeIntoVulkan(VertexStepMode stepMode);

    VkFormat convertVertexFormatIntoVulkan(VertexFormat format);

    VkPrimitiveTopology convertPrimitiveTopologyIntoVulkan(PrimitiveTopology topology);

    VkFrontFace convertFrontFaceIntoVulkan(FrontFace frontFace);

    VkCullModeFlagBits convertCullModeIntoVulkan(CullMode mode);

    VkPolygonMode convertPolygonModeIntoVulkan(PolygonMode mode);

    VkStencilOp convertStencilOpIntoVulkan(StencilOperation stencilOp);

    VkLogicOp convertLogicOpIntoVulkan(LogicOp logicOp);

    VkBlendOp convertBlendOperationIntoVulkan(BlendOperation blendOp);

    VkBlendFactor convertBlendFactorIntoVulkan(BlendFactor blendFactor);

    VkColorComponentFlags convertColorComponentIntoVulkan(ColorWriteFlags colorWrite);

    std::vector<VkDynamicState> convertDynamicStatesIntoVulkan(DynamicStateEnabledState enabledState);

    VkResolveModeFlagBits convertResolveModeIntoVulkan(ResolveMode resolveMode);

    VkAttachmentLoadOp convertLoadOpIntoVulkan(LoadOp loadOp);

    VkAttachmentStoreOp convertStoreOpIntoVulkan(StoreOp storeOp);

    VkIndexType convertIndexFormatIntoVulkan(IndexFormat format);

    VkPipelineStageFlags convertPipelineStageIntoVulkan(PipelineStageFlags stage);

    VkAccessFlags convertBufferAccessIntoVulkan(PipelineStageFlags stage, ResourceAccess access, Buffer *buffer);

    VkAccessFlags convertTextureAccessIntoVulkan(PipelineStageFlags stage, ResourceAccess access, Texture *texture);
}