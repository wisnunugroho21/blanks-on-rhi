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
            std::vector<std::shared_ptr<Queue>> q,
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
          descriptorPool{dsp}
        {

        }
        
        VkDevice getNative() { return this->device; }
        VmaAllocator getMemoryAllocator() { return this->memoryAllocator; }
        VkDescriptorPool getDescriptorPool() { return this->descriptorPool; }

        std::shared_ptr<Buffer> createBuffer(BufferDescriptor descriptor) override;
        std::shared_ptr<Texture> createTexture(TextureDescriptor descriptor) override;
        std::shared_ptr<Sampler> createSampler(SamplerDescriptor desc) override;
        std::shared_ptr<BindGroupLayout> createBindGroupLayout(BindGroupLayoutDescriptor desc) override;
        std::shared_ptr<BindGroup> createBindGroup(BindGroupDescriptor desc) override;
        std::shared_ptr<PipelineLayout> createPipelineLayout(PipelineLayoutDescriptor desc) override;
        std::shared_ptr<ShaderModule> createShaderModule(ShaderModuleDescriptor desc) override;
        std::shared_ptr<ComputePipeline> createComputePipeline(ComputePipelineDescriptor desc) override;
        std::shared_ptr<RenderPipeline> createRenderPipeline(RenderPipelineDescriptor desc) override;
        
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDeviceProperties deviceProperties;
        VmaAllocator memoryAllocator;

        std::vector<std::shared_ptr<Queue>> queues;
        std::vector<VkCommandPool> commandPools;
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

        VkImage getNative() { return this->image; }
        VmaAllocation getMemoryAllocation() { return this->memoryAllocation; }

    protected:
        TextureDescriptor desc;

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

    class VulkanBindGroupLayout : public BindGroupLayout {
    public:
        VulkanBindGroupLayout(
            BindGroupLayoutDescriptor desc,
            VulkanDevice* d,
            VkDescriptorSetLayout dsl
        )
        : desc{desc},
          device{d},
          descSetLayout{dsl}
        {

        }

        ~VulkanBindGroupLayout();

        BindGroupLayoutDescriptor getDesc() override { return this->desc; }

        VkDescriptorSetLayout getNative() { return this->descSetLayout; }

    protected:
        BindGroupLayoutDescriptor desc;
    
    private:
        VulkanDevice* device;
        VkDescriptorSetLayout descSetLayout;
    };

    class VulkanBindGroup : public BindGroup {
    public:
        VulkanBindGroup(
            BindGroupDescriptor desc,
            VulkanDevice* d,
            VkDescriptorSet ds
        )
        : desc{desc},
          device{d},
          descSet{ds}
        {

        }

        BindGroupDescriptor getDesc() override { return this->desc; }

        VkDescriptorSet getNative() { return this->descSet; }

    protected:
        BindGroupDescriptor desc;

    private:
        VulkanDevice* device;
        VkDescriptorSet descSet;
    };

    class VulkanPipelineLayout : public PipelineLayout {
    public:
        VulkanPipelineLayout(
            PipelineLayoutDescriptor desc,
            VulkanDevice* d,
            VkPipelineLayout pl
        )
        : desc{desc},
          device{d},
          pipelineLayout{pl}
        {

        }

        ~VulkanPipelineLayout();

        PipelineLayoutDescriptor getDesc() override { return this->desc; }

        VkPipelineLayout getNative() { return this->pipelineLayout; }

    protected: 
        PipelineLayoutDescriptor desc;

    private:
        VulkanDevice* device;
        VkPipelineLayout pipelineLayout;
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

    class VulkanComputePipeline : public ComputePipeline {
    public:
        VulkanComputePipeline(
            ComputePipelineDescriptor desc,
            VulkanDevice* d,
            VkPipeline p
        )
        : desc{desc},
          device{d},
          pipeline{p}
        {

        }

        ~VulkanComputePipeline();

        ComputePipelineDescriptor getDesc() override { return this->desc; }

        BindGroupLayout* getBindGroupLayout(Uint32 index) override;

        VkPipeline getNative() { return this->pipeline; }

    protected:
        ComputePipelineDescriptor desc;

    private:
        VulkanDevice* device;
        VkPipeline pipeline;
    };

    class VulkanRenderPipeline : public RenderPipeline {
    public:
        VulkanRenderPipeline(
            RenderPipelineDescriptor desc,
            VulkanDevice* d,
            VkPipeline p
        )
        : desc{desc},
          device{d},
          pipeline{p}
        {

        }

        ~VulkanRenderPipeline();

        RenderPipelineDescriptor getDesc() override { return this->desc; }

        BindGroupLayout* getBindGroupLayout(Uint32 index) override;

        VkPipeline getNative() { return this->pipeline; }

    protected:
        RenderPipelineDescriptor desc;

    private:
        VulkanDevice* device;
        VkPipeline pipeline;
    };

    class VulkanRenderPassEncoder : public RenderPassEncoder {
    public:
        VulkanRenderPassEncoder(
            RenderPassDescriptor desc,
            CommandEncoder* c
        )
        : desc{desc},
          commandEncoder{c}
        {

        }

        void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
        void setViewport(Viewport viewport) override;
        void setViewport(std::vector<Viewport> viewports) override;

        void setScissorRect(Uint32 x, Uint32 y, Uint32 width, Uint32 height) override;
        void setScissorRect(Rect2D scissor) override;
        void setScissorRect(std::vector<Rect2D> scissors) override;

        void setLineWidth(float lineWidth) override;

        void setDepthBias(float constant, float clamp, float slopeScale) override;
        void setDepthBias(DepthBias depthBias) override;
        
        void setBlendConstant(float r, float g, float b, float a) override;
        void setBlendConstant(Color color) override;

        void setDepthBounds(float min, float max) override;

        void setStencilCompareMask(Uint32 compareMask) override;

        void setStencilWriteMask(Uint32 writeMask) override;

        void setStencilReference(Uint32 reference) override;

        void end() override;

    protected:
        RenderPassDescriptor desc;
        CommandEncoder* commandEncoder;
    };

    class VulkanCommandEncoder : public CommandEncoder {
    public:
        VulkanCommandEncoder(
            VulkanDevice* d,
            VkCommandBuffer c
        )
        : device{d},
          commandBuffer{c}
        {

        }

        std::shared_ptr<RenderPassEncoder> beginRenderPass(RenderPassDescriptor desc) override;

        VkCommandBuffer getNative() { return this->commandBuffer; }

    protected:
        VulkanDevice* device;
        VkCommandBuffer commandBuffer;
    };

    class VulkanQueue : public Queue {
    public:
        VulkanQueue(
            QueueDescriptor desc,
            VkQueue q,
            Uint32 fi
        )
        : desc{desc},
          queue{q},
          familyIndex{fi}
        {

        }

        QueueDescriptor getDesc() override { return this->desc; }

        void submit(std::vector<CommandEncoder*> commandBuffers) override;

        VkQueue getNative() { return this->queue; }

        Uint32 getFamilyIndex() { return this->familyIndex; }

    protected:
        QueueDescriptor desc;

    private:
        VkQueue queue;
        Uint32 familyIndex;
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
}