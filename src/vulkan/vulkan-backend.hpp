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
        
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDeviceProperties deviceProperties;
        VmaAllocator memoryAllocator;

        std::vector<std::shared_ptr<Queue>> queues;
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

    protected:
        BindGroupDescriptor desc;

    private:
        VulkanDevice* device;
        VkDescriptorSet descSet;
    };

    class VulkanQueue : public Queue {
    public:
        VulkanQueue(
            QueueDescriptor desc,
            VkQueue q
        )
        : Queue(desc),
          queue{q}
        {}

    void submit(std::vector<CommandEncoder*> commandBuffers) override;

    private:
        VkQueue queue;
    };    

    class VulkanFactory {
        static std::shared_ptr<Device> createDevice(DeviceDescriptor desc);
    };
}