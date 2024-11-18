#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace RHI {
    // ===========================================================================================================================
    // Class Definition
    // ===========================================================================================================================

    class Buffer;
    class Texture;
    class TextureView;
    class Sampler;
    class BindGroupLayout;
    class BindGroup;
    class PipelineLayout;
    class ShaderModule;
    class PipelineBase;
    class ProgrammableStage;
    class Queue;
    class CommandBuffer;
    class CommandsMixin;
    class RenderPassEncoder;
    class ComputePassEncoder;
    class RenderBundle;
    class Queue;
    class Device;
    class Adapter;

    struct RenderPassColorAttachment;
    struct RenderPassDepthStencilAttachment;
    struct RenderPassDescriptor;
    struct ComputePassDescriptor;

    // ===========================================================================================================================
    // Basic Type
    // ===========================================================================================================================
    typedef const char* String;
    typedef unsigned short Uint8;
    typedef unsigned int Uint16;
    typedef unsigned long Uint32;
    typedef unsigned long long Uint64;

    typedef int Int16;
    typedef long Int32;
    typedef long long Int64;

    typedef float Float32;
    typedef double Float64;

    typedef unsigned long FlagsConstant;
    typedef unsigned long TextureUsageFlags;
    typedef unsigned long BufferUsageFlags;
    typedef unsigned long ShaderStageFlags;
    typedef unsigned long ColorWriteFlags;
    typedef unsigned long ResolveModeFlags;

    struct Color {
        float r;
        float g;
        float b;
        float a;

        Color() : r{0.0f}, g{0.0f}, b{0.0f}, a{0.0f} {}
        Color(float value) : r{value}, g{value}, b{value}, a{value} {}
        Color(float _r, float _g, float _b, float _a) : r{_r}, g{_g}, b{_b}, a{_a} {}

        bool operator == (const Color& other) const { 
            return this->r == other.r &&
                   this->g == other.g &&
                   this->b == other.b &&
                   this->a == other.a;
        }

		bool operator !=(const Color& other) const { return !(*this == other); }
    };

	struct Origin3D {
        Uint32 x;
        Uint32 y;
        Uint32 z;

		Origin3D() : x{0}, y{0}, z{0} {}
		Origin3D(Uint32 _x, Uint32 _y, Uint32 _z) : x{_x}, y{_y}, z{_z} {}

        bool operator == (const Origin3D& other) const { 
            return this->x == other.x &&
                   this->y == other.y &&
                   this->z == other.z;
        }

		bool operator !=(const Origin3D& other) const { return !(*this == other); }
    };

    struct Extent3D {
        Uint32 width;
        Uint32 height = 1;
        Uint32 depth = 1;

		constexpr Extent3D& setWidth(Uint32 value) { this->width = value; return *this; }
		constexpr Extent3D& setHeight(Uint32 value) { this->height = value; return *this; }
		constexpr Extent3D& setDepth(Uint32 value) { this->depth = value; return *this; }
    };

    struct Rect2D {
        Int32 x;
        Int32 y;
        Uint32 width;
        Uint32 height;

        constexpr Rect2D& setX(Int32 value) { this->x = value; return *this; }
        constexpr Rect2D& setY(Int32 value) { this->y = value; return *this; }
        constexpr Rect2D& setWidth(Uint32 value) { this->width = value; return *this; }
        constexpr Rect2D& setHeight(Uint32 value) { this->height = value; return *this; }
    };

    // ===========================================================================================================================
    // Buffer
    // ===========================================================================================================================

    enum class BufferUsage : FlagsConstant {
        eCopySrc         = 0x0001,
        eCopyDst         = 0x0002,
        eIndex           = 0x0004,
        eVertex          = 0x0008,
        eUniform         = 0x0010,
        eStorage         = 0x0020,
        eIndirect        = 0x0040,
        eQueryResolve    = 0x0080
    };

    enum class BufferLocation : Uint8 {
        eDeviceLocal,
        eHost
    };

    enum class BufferMapState : Uint8 {
        eUnmapped,
        eMapped
    };
    
    struct BufferDescriptor {
        Uint64 size;
        BufferUsageFlags usage;
        BufferLocation location;

		constexpr BufferDescriptor& setSize(Uint64 value) { this->size = value; return *this; }
		constexpr BufferDescriptor& setUsage(BufferUsageFlags value) { this->usage = value; return *this; }
		constexpr BufferDescriptor& setLocation(BufferLocation value) { this->location = value; return *this; }
    };

    class Buffer {
    public:
        virtual BufferDescriptor getDesc() = 0;

        virtual void* getCurrentMapped() = 0;
		virtual BufferMapState getMapState() = 0;

        virtual void insertData(void* pointerData, Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;
        virtual void takeData(void* pointerData, Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;

        virtual void* map() = 0;
        virtual void unmap() = 0;

        virtual void flush(Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;
        virtual void invalidate(Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;
    };

    // ===========================================================================================================================
    // Texture
    // ===========================================================================================================================

    enum TextureFormat : Uint8 {
        // 8-bit formats
        eR8Unorm,
        eR8Snorm,
        eR8Uint,
        eR8Sint,

        // 16-bit formats
        eR16Uint,
        eR16Sint,
        eR16Float,
        eRG8Unorm,
        eRG8Snorm,
        eRG8Uint,
        eRG8Sint,

        // 32-bit formats
        eR32Uint,
        eR32Sint,
        eR32Float,
        eRG16Uint,
        eRG16Sint,
        eRG16Float,
        eRGBA8Unorm,
        eRGBA8UnormSrgb,
        eRGBA8Snorm,
        eRGBA8Uint,
        eRGBA8Sint,
        eBGRA8Unorm,
        eBGRA8UnormSrgb,

        // Packed 32-bit formats
        eRGB9E5Ufloat,
        eRGB10A2Uint,
        eRGB10A2Unorm,

        // 64-bit formats
        eRG32Uint,
        eRG32Sint,
        eRG32Float,
        eRGBA16Uint,
        eRGBA16Sint,
        eRGBA16Float,

        // 128-bit formats
        eRGBA32Uint,
        eRGBA32Sint,
        eRGBA32Float,

        // Depth/stencil formats
        eS8Uint,
        eD16Unorm,
        eD24Plus,
        eD24PlusS8Uint,
        eD32Sfloat,

        // "depth32float-stencil8" feature
        eD32SFloatS8Uint,

        // BC compressed formats usable if "texture-compression-bc" is both
        // supported by the device/user agent and enabled in requestDevice.
        eBC1RGBAUnorm,
        eBC1RGBAUnormSrgb,
        eBC2RGBAUnorm,
        eBC2RGBAUnormSrgb,
        eBC3RGBAUnorm,
        eBC3RGBAUnormSrgb,
        eBC4RUnorm,
        eBC4RSnorm,
        eBC5RGUnorm,
        eBC5RGSnorm,
        eBC6HRGBUfloat,
        eBC6HRGBSfloat,
        eBC7RGBAUnorm,
        eBC7RGBAUnormSrgb,

        // ETC2 compressed formats usable if "texture-compression-etc2" is both
        // supported by the device/user agent and enabled in requestDevice.
        eETC2RGB8Unorm,
        eETC2RGB8UnormSrgb,
        eETC2RGB8A1Unorm,
        eETC2RGB8A1UnormSrgb,
        eETC2RGBA8Unorm,
        eETC2RGBA8UnormSrgb,
        eEACR11Unorm,
        eEACR11Snorm,
        eEACRG11Unorm,
        eEACRG11Snorm,

        // ASTC compressed formats usable if "texture-compression-astc" is both
        // supported by the device/user agent and enabled in requestDevice.
        eASTC4X4Unorm,
        eASTC4X4UnormSrgb,
        eASTC5X4Unorm,
        eASTC5X4UnormSrgb,
        eASTC5X5Unorm,
        eASTC5X5UnormSrgb,
        eASTC6X5Unorm,
        eASTC6X5UnormSrgb,
        eASTC6X6Unorm,
        eASTC6X6UnormSrgb,
        eASTC8X5Unorm,
        eASTC8X5UnormSrgb,
        eASTC8X6Unorm,
        eASTC8X6UnormSrgb,
        eASTC8X8Unorm,
        eASTC8X8UnormSrgb,
        eASTC10X5Unorm,
        eASTC10X5UnormSrgb,
        eASTC10X6Unorm,
        eASTC10X6UnormSrgb,
        eASTC10X8Unorm,
        eASTC10X8UnormSrgb,
        eASTC10X10Unorm,
        eASTC10X10UnormSrgb,
        eASTC12X10Unorm,
        eASTC12X10UnormSrgb,    
        eASTC12X12Unorm,
        eASTC12X12UnormSrgb
    };

    enum class TextureDimension : Uint8 {
        e1D,
        e2D,
        e3D
    };

    enum class TextureViewDimension : Uint8 {
        e1D,
        e2D,
        e2DArray,
        eCube,
        eCubeArray,
        e3D
    };

    enum class TextureAspect : Uint8 {
        eColor,
        eDepth, 
        eStencil,
        eDepthStencil
    };

    enum class TextureUsage : FlagsConstant {
        eCopySrc                = 0x01,
        eCopyDst                = 0x02,
        eTextureBinding         = 0x04,
        eStorageBinding         = 0x08,
        eColorAttachment        = 0x10,
        eDepthStencilAttachment = 0x20
    };

    enum class TextureState : Uint8 {
        eUndefined,
        eColorAttachment,
        eDepthAttachment,
        eStencilAttachment,
        eColorTextureBinding,
        eDepthStencilTextureBinding,
        eStorageBinding,
        eCopySrc,
        eCopyDst,
        ePresent
    };

    struct TextureDescriptor {
        Extent3D size;
        TextureFormat format;
        TextureUsageFlags usage;

        Uint32 sliceLayersNum = 1;
        Uint32 mipLevelCount = 1;
        Uint32 sampleCount = 1;

        TextureDimension dimension = TextureDimension::e2D;
        TextureState initialState = TextureState::eUndefined;

        constexpr TextureDescriptor& setSize(Extent3D value) { this->size = value; return *this; }
        constexpr TextureDescriptor& setFormat(TextureFormat value) { this->format = value; return *this; }
        constexpr TextureDescriptor& setUsage(TextureUsageFlags value) { this->usage = value; return *this; }
        constexpr TextureDescriptor& setSliceLayersNum(Uint32 value) { this->sliceLayersNum = value; return *this; }
        constexpr TextureDescriptor& setMipLevelCount(Uint32 value) { this->mipLevelCount = value; return *this; }
        constexpr TextureDescriptor& setSampleCount(Uint32 value) { this->sampleCount = value; return *this; }
        constexpr TextureDescriptor& setDimension(TextureDimension value) { this->dimension = value; return *this; }
        constexpr TextureDescriptor& setInitialState(TextureState value) { this->initialState = value; return *this; }
    };

    struct TextureSubresource {
        TextureAspect aspect = TextureAspect::eColor;

        Uint32 baseMipLevel = 0;
        Uint32 mipLevelCount = 1;

        Uint32 baseArrayLayer = 0;
        Uint32 arrayLayerCount = 1;

        constexpr TextureSubresource& setAspect(TextureAspect value) { this->aspect = value; return *this; }
        constexpr TextureSubresource& setBaseMipLevel(Uint32 value) { this->baseMipLevel = value; return *this; }
        constexpr TextureSubresource& setMipLevelCount(Uint32 value) { this->mipLevelCount = value; return *this; }
        constexpr TextureSubresource& setBaseArrayLayer(Uint32 value) { this->baseArrayLayer = value; return *this; }
        constexpr TextureSubresource& setArrayLayerCount(Uint32 value) { this->arrayLayerCount = value; return *this; }
    };

    struct TextureViewDescriptor {
        TextureSubresource subresource;
        TextureFormat format;

        TextureViewDimension dimension = TextureViewDimension::e2D;

        constexpr TextureViewDescriptor& setSubresource(TextureSubresource value) { this->subresource = value; return *this; }
        constexpr TextureViewDescriptor& setFormat(TextureFormat value) { this->format = value; return *this; }
        constexpr TextureViewDescriptor& setDimension(TextureViewDimension value) { this->dimension = value; return *this; }
    };

    class Texture {
    public:
        virtual TextureDescriptor getDesc() = 0;
        virtual std::shared_ptr<TextureView> createView(TextureViewDescriptor desc) = 0;
    };

    class TextureView {
    public:
        virtual TextureViewDescriptor getDesc() = 0;
        virtual Texture* getTexture() = 0;
    };

    // ===========================================================================================================================
    // Sampler
    // ===========================================================================================================================

    enum class AddressMode : Uint8 {
        eRepeat,
        eMirrorRepeat,
        eClampToEdge,
        eClampToBorder
    };

    enum class FilterMode : Uint8 {
        eNearest,
        eLinear
    };

    enum class MipmapFilterMode : Uint8 {
        eNearest,
        eLinear
    };

    enum class CompareFunction : Uint8 {
        eNever,
        eEqual,
        eLess,
        eLessEqual,
        eGreater,
        eNotEqual,
        eGreaterEqual,
        eAlways,
    };

    enum class BorderColor : Uint8 {
        eFloatTransparentBlack,
        eIntTransparentBlack,
        eFloatOpaqueBlack,
        eIntOpaqueBlack,
        eFloatOpaqueWhite,
        eIntOpaqueWhite
    };

    struct SamplerDescriptor {
        AddressMode addressModeU = AddressMode::eClampToBorder;
        AddressMode addressModeV = AddressMode::eClampToBorder;
        AddressMode addressModeW = AddressMode::eClampToBorder;
        FilterMode magFilter = FilterMode::eNearest;
        FilterMode minFilter = FilterMode::eNearest;
        MipmapFilterMode mipmapFilter = MipmapFilterMode::eNearest;
        CompareFunction compare = CompareFunction::eNever;
        BorderColor borderColor = BorderColor::eFloatOpaqueBlack;

        Float32 lodMinClamp = 0;
        Float32 lodMaxClamp = 32;
        Uint32 maxAnisotropy = 1;

        constexpr SamplerDescriptor& setAddressModeU(AddressMode value) { this->addressModeU = value; return *this; }
        constexpr SamplerDescriptor& setAddressModeV(AddressMode value) { this->addressModeV = value; return *this; }
        constexpr SamplerDescriptor& setAddressModeW(AddressMode value) { this->addressModeW = value; return *this; }
        constexpr SamplerDescriptor& setMagFilter(FilterMode value) { this->magFilter = value; return *this; }
        constexpr SamplerDescriptor& setMinFilter(FilterMode value) { this->minFilter = value; return *this; }
        constexpr SamplerDescriptor& setMipmapFilter(MipmapFilterMode value) { this->mipmapFilter = value; return *this; }
        constexpr SamplerDescriptor& setCompare(CompareFunction value) { this->compare = value; return *this; }
        constexpr SamplerDescriptor& setBorderColor(BorderColor value) { this->borderColor = value; return *this; }
        constexpr SamplerDescriptor& setLodMinClamp(Float32 value) { this->lodMinClamp = value; return *this; }
        constexpr SamplerDescriptor& setLodMaxClamp(Float32 value) { this->lodMaxClamp = value; return *this; }
        constexpr SamplerDescriptor& setMaxAnisotropy(Uint32 value) { this->maxAnisotropy = value; return *this; }
    };

    class Sampler {
    public:
        virtual SamplerDescriptor getDesc() = 0;

        virtual bool isComparison() = 0;
        virtual bool isFiltering() = 0;
    };

    // ===========================================================================================================================
    // Resource Binding
    // ===========================================================================================================================

    enum class ShaderStage : FlagsConstant {
        eCompute        = 0x0001,
        eVertex         = 0x0002,
        eFragment       = 0x0004,
        eTessellCtrl    = 0x0008,
        eTessellEval    = 0x0010,
        eTask           = 0x0020,
        eMesh           = 0x0040,
    };

    enum class BindingType : Uint8 {
        eUniformBuffer,
        eStorageBuffer,
        eSampledTexture,
        eStorageTexture,
        eSampler
    };

    struct BindGroupLayoutEntry {
        Uint32 binding;
        ShaderStageFlags shaderStage;
        BindingType type;

        Uint32 bindCount = 1;

        constexpr BindGroupLayoutEntry& setBinding(Uint32 value) { this->binding = value; return *this; }
        constexpr BindGroupLayoutEntry& setShaderStage(ShaderStageFlags value) { this->shaderStage = value; return *this; }
        constexpr BindGroupLayoutEntry& setType(BindingType value) { this->type = value; return *this; }
        constexpr BindGroupLayoutEntry& setBindCount(Uint32 value) { this->bindCount = value; return *this; }
    };

    struct BindGroupLayoutDescriptor {
        std::map<Uint32, BindGroupLayoutEntry> entries;

        constexpr BindGroupLayoutDescriptor& setEntries(const std::vector<BindGroupLayoutEntry>& value) { 
            for (auto &&item : value) {
                this->entries[item.binding] = item;
            }
             
            return *this;
        }

        constexpr BindGroupLayoutDescriptor& addEntry(BindGroupLayoutEntry item) { this->entries[item.binding] = item; return *this; }
        constexpr BindGroupLayoutDescriptor& addEntry(Uint32 binding, ShaderStageFlags shaderStage, BindingType type, Uint32 bindCount = 1) { 
            this->entries[binding] = 
                BindGroupLayoutEntry()
                    .setBinding(binding)
                    .setShaderStage(shaderStage)
                    .setType(type)
                    .setBindCount(bindCount);

            return *this; 
        }
    };

    class BindGroupLayout {
    public:
        virtual BindGroupLayoutDescriptor getDesc() = 0;
    };

    struct BindGroupEntry {
        Uint32 binding;

        virtual BindGroupEntry& setBinding(Uint32 value) = 0;
    };

    struct BufferBindGroupItem {
        Buffer* buffer;
        Uint64 size = ULLONG_MAX;
        Uint64 offset = 0;

        constexpr BufferBindGroupItem& setBuffer(Buffer* value) { this->buffer = value; return *this; }
        constexpr BufferBindGroupItem& setSize(Uint64 value) { this->size = value; return *this; }
        constexpr BufferBindGroupItem& setOffset(Uint64 value) { this->offset = value; return *this; }
    };

    struct TextureBindGroupItem {
        TextureView* textureView;

        constexpr TextureBindGroupItem& setTextureView(TextureView* value) { this->textureView = value; return *this; }
    };

    struct SamplerBindGroupItem {
        Sampler* sampler;

        constexpr SamplerBindGroupItem& setSampler(Sampler* value) { this->sampler = value; return *this; }
    };

    struct BufferBindGroupEntry : BindGroupEntry {
        std::vector<BufferBindGroupItem> groupItems;

        BindGroupEntry& setBinding(Uint32 value) override { this->binding = value; return *this; }

        constexpr BufferBindGroupEntry& setEntries(const std::vector<BufferBindGroupItem>& value) { this->groupItems = value; return *this; }

        constexpr BufferBindGroupEntry& addEntry(BufferBindGroupItem item) { this->groupItems.emplace_back(item); return *this; }

        constexpr BufferBindGroupEntry& addEntry(Buffer* buffer, Uint64 size = ULLONG_MAX, Uint64 offset = 0) {
            this->groupItems.emplace_back(
                BufferBindGroupItem()
                    .setBuffer(buffer)
                    .setSize(size)
                    .setOffset(offset)
            );

            return *this;
        }
    };

    struct TextureBindGroupEntry : BindGroupEntry {
        std::vector<TextureBindGroupItem> groupItems;

        BindGroupEntry& setBinding(Uint32 value) override { this->binding = value; return *this; }

       constexpr TextureBindGroupEntry& setEntries(const std::vector<TextureBindGroupItem>& value) { this->groupItems = value; return *this; }
       
       constexpr TextureBindGroupEntry& addEntry(TextureBindGroupItem item) { this->groupItems.emplace_back(item); return *this; }

        constexpr TextureBindGroupEntry& addEntry(TextureView* textureView) { 
            this->groupItems.emplace_back(
                TextureBindGroupItem()
                    .setTextureView(textureView)
            );

            return *this; 
        }
    };

    struct SamplerBindGroupEntry : BindGroupEntry {
        std::vector<SamplerBindGroupItem> groupItems;

        BindGroupEntry& setBinding(Uint32 value) override { this->binding = value; return *this; }

        constexpr SamplerBindGroupEntry& setEntries(const std::vector<SamplerBindGroupItem>& value) { this->groupItems = value; return *this; }
       
        constexpr SamplerBindGroupEntry& addEntry(SamplerBindGroupItem item) { this->groupItems.emplace_back(item); return *this; }

        constexpr SamplerBindGroupEntry& addEntry(Sampler* sampler) {
            this->groupItems.emplace_back(
                SamplerBindGroupItem()
                    .setSampler(sampler)
            );

            return *this;
        }
    };

    struct BindGroupDescriptor {
        BindGroupLayout* layout;
        std::map<Uint32, BindGroupEntry*> entries;

        constexpr BindGroupDescriptor& setLayout(BindGroupLayout* value) { this->layout = value; return *this; }
        
        constexpr BindGroupDescriptor& addBuffer(Uint32 binding, Buffer* buffer, Uint64 size = ULLONG_MAX, Uint64 offset = 0) {
            BufferBindGroupEntry* groupEntry = new BufferBindGroupEntry();
            groupEntry->setBinding(binding);
            groupEntry->addEntry(buffer, size, offset);

            this->entries[binding] = groupEntry;
            return *this;
        }

        constexpr BindGroupDescriptor& addBuffer(Uint32 binding, const std::vector<BufferBindGroupItem>& entries) {
            BufferBindGroupEntry* groupEntry = new BufferBindGroupEntry();
            groupEntry->setBinding(binding);
            groupEntry->setEntries(entries);

            this->entries[binding] = groupEntry;
            return *this;
        }

        constexpr BindGroupDescriptor& addTextureView(Uint32 binding, TextureView* textureView) {
            TextureBindGroupEntry* groupEntry = new TextureBindGroupEntry();
            groupEntry->setBinding(binding);
            groupEntry->addEntry(textureView);

            this->entries[binding] = groupEntry;
            return *this;
        }

        constexpr BindGroupDescriptor& addTextureView(Uint32 binding, const std::vector<TextureBindGroupItem>& entries) {
            TextureBindGroupEntry* groupEntry = new TextureBindGroupEntry();
            groupEntry->setBinding(binding);
            groupEntry->setEntries(entries);

            this->entries[binding] = groupEntry;
            return *this;
        }

        constexpr BindGroupDescriptor& addSampler(Uint32 binding, Sampler* sampler) {
            SamplerBindGroupEntry* groupEntry = new SamplerBindGroupEntry();
            groupEntry->setBinding(binding);
            groupEntry->addEntry(sampler);

            this->entries[binding] = groupEntry;
            return *this;
        }

        constexpr BindGroupDescriptor& addSampler(Uint32 binding, const std::vector<SamplerBindGroupItem>& entries) {
            SamplerBindGroupEntry* groupEntry = new SamplerBindGroupEntry();
            groupEntry->setBinding(binding);
            groupEntry->setEntries(entries);

            this->entries[binding] = groupEntry;
            return *this;
        }
    };

    struct ConstantLayout {
        ShaderStageFlags shaderStage;
        Uint64 size = ULLONG_MAX;
        Uint64 offset = 0;

        constexpr ConstantLayout& setShaderStage(ShaderStageFlags value) { this->shaderStage = value; return *this; }
        constexpr ConstantLayout& setSize(Uint64 value) { this->size = value; return *this; }
        constexpr ConstantLayout& setOffset(Uint64 value) { this->offset = value; return *this; }
    };

    struct PipelineLayoutDescriptor {
        std::vector<BindGroupLayout*> bindGroupLayouts;
        std::vector<ConstantLayout> constantLayouts;

        constexpr PipelineLayoutDescriptor& setBindGroups(const std::vector<BindGroupLayout*>& value) { this->bindGroupLayouts = value; return *this; }
        constexpr PipelineLayoutDescriptor& setConstants(const std::vector<ConstantLayout>& value) { this->constantLayouts = value; return *this; }

        constexpr PipelineLayoutDescriptor& addBindGroup(BindGroupLayout* item) { this->bindGroupLayouts.emplace_back(item); return *this; }
        constexpr PipelineLayoutDescriptor& addConstant(ConstantLayout item) { this->constantLayouts.emplace_back(item); return *this; }

        constexpr PipelineLayoutDescriptor& addConstant(ShaderStageFlags shaderStage, Uint64 size, Uint64 offset) {
            this->constantLayouts.emplace_back(
                ConstantLayout()
                    .setShaderStage(shaderStage)
                    .setSize(size)
                    .setOffset(offset)
            );

            return *this;
        }
    };

    class BindGroup {
    public:
        virtual BindGroupDescriptor getDesc() =  0;
    };

    class PipelineLayout {
    public:
        virtual PipelineLayoutDescriptor getDesc() = 0;
    };

    // ===========================================================================================================================
    // Shader Module
    // ===========================================================================================================================

    struct ShaderModuleDescriptor {
        String filename;
    };

    class ShaderModule {
    public:
        virtual ShaderModuleDescriptor getDesc() = 0;
    };

    // ===========================================================================================================================
    // Pipeline
    // ===========================================================================================================================

    enum class VertexStepMode : Uint8 {
        eVertex,
        eInstance
    };

    enum class VertexFormat : Uint8 {
        eUint8x2,
        eUint8x4,
        eSint8x2,
        eSint8x4,
        eUnorm8x2,
        eUnorm8x4,
        eSnorm8x2,
        eSnorm8x4,
        eUint16x2,
        eUint16x4,
        eSint16x2,
        eSint16x4,
        eUnorm16x2,
        eUnorm16x4,
        eSnorm16x2,
        eSnorm16x4,
        eFloat16x2,
        eFloat16x4,
        eFloat32,
        eFloat32x2,
        eFloat32x4,
        eUint32,
        eUint32x2,
        eUint32x4,
        eSint32,
        eSint32x2,
        eSint32x4
    };

    enum class PrimitiveTopology : Uint8 {
        ePointList,
        eLineList,
        eLineStrip,
        eTriangleList,
        eTriangleStrip
    };

    enum class IndexFormat : Uint8 {
        eUint16,
        eUint32
    };

    enum class FrontFace : Uint8 {
        eCCW,
        eCW
    };

    enum class CullMode : Uint8 {
        eNone,
        eFront,
        eBack,
        eAll
    };

    enum class PolygonMode : Uint8 {
        eFill,
        eLine,
        ePoint
    };

    enum StencilOperation : Uint8 {
        eKeep,
        eZero,
        eReplace,
        eInvert,
        eIncrementClamp,
        eDecrementClamp
    };

    enum class BlendOperation : Uint8 {
        eAdd,
        eSubtract,
        eReverseSubtract,
        eMin,
        eMax
    };

    enum class BlendFactor : Uint8 {
        eZero,
        eOne,
        eSrc,
        eOneMinusSrc,
        eSrcAlpha,
        eOneMinusSrcAlpha,
        eDst,
        eOneMinusDst,
        eDstAlpha,
        eOneMinusDstAlpha,
        eSrcAlphaSaturated,
        eConstant,
        eOneMinusConstant,
        eSrc1,
        eOneMinusSrc1,
        eSrc1Alpha,
        eOneMinusSrc1Alpha,
    };

    enum class ColorWrite : FlagsConstant {
        eRed   = 0x1,
        eGreen = 0x2,
        eBlue  = 0x4,
        eAlpha = 0x8,
        eAll   = 0xF
    };

    enum class LogicOp : Uint8 {
        eCopy,
        eKeep,
        eClear,
        eAnd,
        eAndReverse,
        eAndInverted,
        eXor,
        eOr,
        eNor,
        eEquivalent,
        eInvert,
        eOrReverse,
        eCopyInverted,
        eOrInverted,
        eNand,
        eSet
    };

    struct ProgrammableStage {
        ShaderModule* module;
    };

    struct VertexAttribute {
        VertexFormat format;
        Uint64 offset;
        Uint32 shaderLocation;
    };

    struct Viewport {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    struct DepthBias {
        float constant = 0.0f;
        float slopeScale = 0.0f;
        float clamp = 0.0f;
    };

    struct BlendComponent {
        BlendOperation operation = BlendOperation::eAdd;
        BlendFactor srcFactor = BlendFactor::eOne;
        BlendFactor dstFactor = BlendFactor::eZero;
    };

    struct BlendState {
        bool blendEnabled = false;
        BlendComponent color{};
        BlendComponent alpha{};
    };

    struct ColorTargetState {
        TextureFormat format;
        
        BlendState blend{};
        ColorWriteFlags colorWriteMask = 0xF;  // ColorWrite.ALL
    };

    struct VertexBufferLayout {
        Uint64 arrayStride;
        VertexStepMode stepMode = VertexStepMode::eVertex;
        std::vector<VertexAttribute> attributes;
    };

    struct VertexState : ProgrammableStage {
        std::vector<VertexBufferLayout> buffers{};
    };

    struct PrimitiveState {
        PrimitiveTopology topology = PrimitiveTopology::eTriangleList;
        IndexFormat stripIndexFormat = IndexFormat::eUint32;
    };

    struct ViewportScissorState {
        std::vector<Viewport> viewports;
        std::vector<Rect2D> scissors;
    };

    struct RasterizationState {
        FrontFace frontFace = FrontFace::eCCW;
        CullMode cullMode = CullMode::eNone;
        PolygonMode polygonMode = PolygonMode::eFill;
        float lineWidth = 1.0f;

        DepthBias depthBias{};
        bool unclippedDepth = false; // Requires "depth-clip-control" feature.
    };

    struct DepthState {
        TextureFormat format;

        bool depthTestEnabled;
        bool depthWriteEnabled;
        CompareFunction depthCompareOp;

        bool depthBoundsTestEnabled;
        float minDepthBounds;
        float maxDepthBounds;
    };

    struct StencilFaceState {
        CompareFunction compareOp = CompareFunction::eAlways;
        StencilOperation failOp = StencilOperation::eKeep;
        StencilOperation depthFailOp = StencilOperation::eKeep;
        StencilOperation passOp = StencilOperation::eKeep;
    };

    struct StencilState {
        TextureFormat format;
        bool stencilTestEnabled;

        StencilFaceState stencilFront{};
        StencilFaceState stencilBack{};

        Uint32 stencilCompareMask = 0xFFFFFFFF;
        Uint32 stencilWriteMask = 0xFFFFFFFF;
        Uint32 stencilRefence = 0xFFFFFFFF;
    };

    struct MultisampleState {
        Uint32 count = 1;
        Uint32 mask = 0xFFFFFFFF;

        bool alphaToCoverageEnabled = false;
        bool alphaToOneEnabled = false;
    };

    struct FragmentState : ProgrammableStage {
        std::vector<ColorTargetState> targets;
        
        LogicOp logicOp = LogicOp::eCopy;
        Color blendConstant{};
    };

    struct DynamicStateEnabledState {
        bool viewport = false;
        bool scissorRect = false;
        bool lineWidth = false;
        bool depthBias = false;
        bool blendConstants = false;
        bool depthBounds = false;
        bool stencilCompareMask = false;
        bool stencilWriteMask = false;
        bool stencilReference = false;
    };

    struct PipelineDescriptorBase {
        PipelineLayout* layout;
    };

    struct ComputePipelineDescriptor : PipelineDescriptorBase {
        ProgrammableStage compute;
    };

    struct RenderPipelineDescriptor : PipelineDescriptorBase {
        VertexState vertex;
        FragmentState fragment;
        DepthState depth;
        StencilState stencil;
        ViewportScissorState viewportScissor;

        PrimitiveState primitive{};
        RasterizationState rasterizationState{};
        MultisampleState multisample{};
        DynamicStateEnabledState dynamicState{};
    };

    class PipelineBase {
        virtual BindGroupLayout* getBindGroupLayout(Uint32 index) = 0;
    };

    class ComputePipeline : public PipelineBase {
    public:
        virtual ComputePipelineDescriptor getDesc() = 0;
    };

    class RenderPipeline : public PipelineBase {
    public:
        virtual RenderPipelineDescriptor getDesc() = 0;

        bool writesDepth;
        bool writesStencil;
    };

    // ===========================================================================================================================
    // Query
    // ===========================================================================================================================

    enum class QueryType : Uint8 {
        Occulusion,
        Timestamp
    };

    struct QuerySetDescriptor {
        QueryType type;
        Uint32 count;
    };

    class QuerySet {    
        QuerySetDescriptor desc;
    };

    // ===========================================================================================================================
    // Copies
    // ===========================================================================================================================

    struct ImageDataLayout {
        Uint64 offset = 0;
        Uint32 bytesPerRow;
        Uint32 rowsPerImage;
    };

    struct ImageCopyTexture {
        Texture* texture;
        Uint32 mipLevel = 0;
        Origin3D origin{};
        TextureAspect aspect = TextureAspect::eColor;
    };

    struct ImageCopyBuffer : ImageDataLayout {
        Buffer* buffer;
    };

    // ===========================================================================================================================
    // Barrier
    // ===========================================================================================================================

    enum class ResourceAccess : Uint8 {
        eWriteOnly,
        eReadOnly,
        eReadWrite
    };
    
    struct MemoryBarrier {
        ResourceAccess srcAccess;
        ResourceAccess dstAccess;
    };

    struct BufferBarrier : MemoryBarrier {
        Buffer* buffer;
        uint64_t size = ULLONG_MAX;
        uint64_t offset = 0;
    };

    struct ImageBarrier : MemoryBarrier {
        Texture *texture;
        TextureSubresource subresource;

        TextureState srcState;
        TextureState dstState;
    };

    class BarrierCommandsMixin {
        virtual void activatePipelineBarrier(
            ShaderStage srcStage,
            ShaderStage dstStage
        ) = 0;

        virtual void activateBufferBarrier(
            ShaderStage srcStage,
            ShaderStage dstStage,
            BufferBarrier desc
        ) = 0;

        virtual void activateImageBarrier(
            ShaderStage srcStage,
            ShaderStage dstStage,
            ImageBarrier desc
        ) = 0;
    };

    // ===========================================================================================================================
    // Command Encoder
    // ===========================================================================================================================

    enum class CommandState : Uint8 {
        eOpen,
        eLocked,
        eEnded
    };

    class CommandsMixin {
        CommandState state;
    };

    class CommandEncoder : public CommandsMixin, public BarrierCommandsMixin {
        virtual std::shared_ptr<RenderPassEncoder> beginRenderPass(RenderPassDescriptor desc) = 0;
        virtual std::shared_ptr<ComputePassEncoder> beginComputePass(ComputePassDescriptor desc) = 0;

        virtual void copyBufferToBuffer(
            Buffer* source,
            Uint64 sourceOffset,
            Buffer* destination,
            Uint64 destinationOffset,
            Uint64 size) = 0;

        virtual void copyBufferToTexture(
            ImageCopyBuffer source,
            ImageCopyTexture destination,
            Extent3D copySize) = 0;

        virtual void copyTextureToBuffer(
            ImageCopyTexture source,
            ImageCopyBuffer destination,
            Extent3D copySize) = 0;

        virtual void copyTextureToTexture(
            ImageCopyTexture source,
            ImageCopyTexture destination,
            Extent3D copySize) = 0;

        virtual void clearBuffer(
            Buffer* buffer,
            Uint64 size = ULLONG_MAX,
            Uint64 offset = 0
        ) = 0;

        virtual void resolveQuerySet(
            QuerySet querySet,
            Uint32 firstQuery,
            Uint32 queryCount,
            Buffer* destination,
            Uint64 destinationOffset) = 0;

        virtual void finish() = 0;
    };

    // ===========================================================================================================================
    // Programmable Passes
    // ===========================================================================================================================

    class BindingCommandsMixin {
        virtual void setBindGroup(Uint32 index, BindGroup* bindGroup, std::vector<Uint32> dynamicOffsets = {}) = 0;

        virtual void setBindGroup(std::vector<BindGroup*>  bindGroup, std::vector<Uint32> dynamicOffsets = {}) = 0;
    };

    // ===========================================================================================================================
    // Compute Passes
    // ===========================================================================================================================

    struct ComputePassTimestampWrites {
        QuerySet* querySet;
        Uint32 beginningOfPassWriteIndex;
        Uint32 endOfPassWriteIndex;
    };

    struct ComputePassDescriptor {
        ComputePassTimestampWrites timestampWrites;
    };

    class ComputePassEncoder : public CommandsMixin, public BindingCommandsMixin {
        ComputePassDescriptor desc;
        CommandEncoder* commandEncoder;

        virtual void setPipeline(ComputePipeline* pipeline) = 0;
        virtual void dispatchWorkgroups(Uint32 workgroupCountX, Uint32 workgroupCountY = 1, Uint32 workgroupCountZ = 1) = 0;
        virtual void dispatchWorkgroupsIndirect(Buffer* indirectBuffer, Uint64 indirectOffset) = 0;

        virtual void end() = 0;
    };

    // ===========================================================================================================================
    // Render Passes
    // ===========================================================================================================================

    enum class LoadOp : Uint8 {
        eLoad,
        eClear
    };

    enum class StoreOp : Uint8 {
        eStore,
        eDiscard
    };

    enum class ResolveMode : FlagsConstant {
        eAverage    = 0x0001,
        eMin        = 0x0002,
        eMax        = 0x0004
    };

    struct RenderPassColorAttachment {
        TextureView* targetView;
        TextureView* resolveTargetView;
        ResolveMode resolveMode;

        Color clearValue;
        LoadOp loadOp;
        StoreOp storeOp;
    };

    struct RenderPassDepthAttachment {
        TextureView* targetView;

        float clearValue;
        LoadOp loadOp;
        StoreOp storeOp;

        bool readOnly = false;
    };

    struct RenderPassStencilAttachment {
        TextureView* targetView;

        Uint32 clearValue;
        LoadOp loadOp;
        StoreOp storeOp;

        bool readOnly = false;
    };

    struct RenderPassTimestampWrites {
        QuerySet* querySet;
        Uint32 beginningOfPassWriteIndex;
        Uint32 endOfPassWriteIndex;
    };

    class RenderCommandsMixin {
        virtual void setPipeline(RenderPipeline* pipeline) = 0;

        virtual void setIndexBuffer(Buffer* buffer, Uint64 offset = 0) = 0;
        virtual void setVertexBuffer(std::vector<Buffer*> buffer, std::vector<Uint64> offsets = {}) = 0;

        virtual void draw(Uint32 vertexCount, Uint32 instanceCount = 1,  Uint32 firstVertex = 0, Uint32 firstInstance = 0) = 0;
        virtual void drawIndexed(Uint32 indexCount, Uint32 instanceCount = 1, Uint32 firstIndex = 0, Int32 baseVertex = 0, 
            Uint32 firstInstance = 0) = 0;

        virtual void drawIndirect(Buffer* indirectBuffer, Uint64 indirectOffset = 0, Uint64 drawCount = 1) = 0;
        virtual void drawIndirectCount(Buffer* indirectBuffer, Uint64 indirectOffset = 0, Buffer* countBuffer, Uint64 countOffset = 0) = 0;

        virtual void drawIndexedIndirect(Buffer* indirectBuffer, Uint64 indirectOffset = 0, Uint64 drawCount = 1) = 0;
        virtual void drawIndexedIndirectCount(Buffer* indirectBuffer, Uint64 indirectOffset = 0, Buffer* countBuffer, Uint64 countOffset = 0) = 0;
    };

    struct RenderPassDescriptor {
        std::vector<RenderPassColorAttachment> colorAttachments;
        RenderPassDepthAttachment depthAttachment;
        RenderPassStencilAttachment stencilAttachment;
        
        QuerySet* occlusionQuerySet;
        RenderPassTimestampWrites timestampWrites;

        Uint64 maxDrawCount = 50000000;
    };

    struct RenderState {
        RenderPipeline* pipeline;

        std::vector<Viewport> viewports;
        std::vector<Rect2D> scissors;
        float lineWidth;
        DepthBias depthBias;
        Color blendConstant;
        float depthBoundMin;
        float depthBoundMax;
        Uint32 stencilCompareMask;
        Uint32 stencilWriteMask;
        Uint32 stencilReference;
    };

    class RenderPassEncoder : public CommandsMixin, public BindingCommandsMixin, public RenderCommandsMixin {
    public:
        virtual RenderPipelineDescriptor getDesc() = 0;
        virtual CommandEncoder* getCommandEncoder() = 0;
        virtual RenderState getCurrentState() = 0;

        virtual void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;
        virtual void setViewport(Viewport viewport) = 0;
        virtual void setViewport(std::vector<Viewport> viewports) = 0;

        virtual void setScissorRect(Int32 x, Int32 y, Uint32 width, Uint32 height) = 0;
        virtual void setScissorRect(Rect2D rect) = 0;
        virtual void setScissorRect(std::vector<Rect2D> rects) = 0;

        virtual void setLineWidth(float lineWidth) = 0;

        virtual void setDepthBias(float constant, float clamp, float slopeScale) = 0;
        virtual void setDepthBias(DepthBias depthBias) = 0;
        
        virtual void setBlendConstant(float r, float g, float b, float a) = 0;
        virtual void setBlendConstant(Color blendConstant) = 0;

        virtual void setDepthBounds(float min, float max) = 0;

        virtual void setStencilCompareMask(Uint32 compareMask) = 0;

        virtual void setStencilWriteMask(Uint32 writeMask) = 0;

        virtual void setStencilReference(Uint32 reference) = 0;

        // virtual void beginOcclusionQuery(Uint32 queryIndex) = 0;
        // virtual void endOcclusionQuery() = 0;

        virtual void end() = 0;
    };

    // ===========================================================================================================================
    // Queue
    // ===========================================================================================================================

    enum class QueueType : uint8_t
    {
        Graphic,
        Compute,
        Transfer
    };

    struct QueueDescriptor {
        QueueType type;
        uint32_t index;

        constexpr QueueDescriptor& setType(QueueType type) { this->type = type; return *this; }
        constexpr QueueDescriptor& setIndex(uint32_t index) { this->index = index; return *this; }
    };

    class Queue {
    public:
        virtual QueueDescriptor getDesc() = 0;
        virtual void submit(std::vector<CommandEncoder*> commandBuffers) = 0;
    }; 

    // ===========================================================================================================================
    // Device
    // ===========================================================================================================================

    struct DeviceInfo {
        String vendor;
        String architecture;
        String device;
    };

    struct SupportedFeatures {
        bool depthClipControl;
        bool depth32floatStencil8float;
        bool textureCompressionBc;
        bool textureCompressionBcSliced3d;
        bool textureCompressionEtc2;
        bool textureCompressionAstc;
        bool textureCompressionAstcSliced3d;
        bool timestampQuery;
        bool indirectFirstInstance;
        bool shaderFloat16;
        bool rg11b10ufloatRender;
        bool bgra8unormStorage;
        bool float32Filterable;
        bool float32Blendable;
        bool clipDistance;
        bool dualSourceBlending;
    };

    struct SupportedLimits {
        unsigned long maxTextureDimension1D;
        unsigned long maxTextureDimension2D;
        unsigned long maxTextureDimension3D;
        unsigned long maxTextureArrayLayers;
        unsigned long maxBindGroups;
        unsigned long maxBindGroupsPlusVertexBuffers;
        unsigned long maxBindingsPerBindGroup;
        unsigned long maxDynamicUniformBuffersPerPipelineLayout;
        unsigned long maxDynamicStorageBuffersPerPipelineLayout;
        unsigned long maxSampledTexturesPerShaderStage;
        unsigned long maxSamplersPerShaderStage;
        unsigned long maxStorageBuffersPerShaderStage;
        unsigned long maxStorageTexturesPerShaderStage;
        unsigned long maxUniformBuffersPerShaderStage;
        unsigned long long maxUniformBufferBindingSize;
        unsigned long long maxStorageBufferBindingSize;
        unsigned long minUniformBufferOffsetAlignment;
        unsigned long minStorageBufferOffsetAlignment;
        unsigned long maxVertexBuffers;
        unsigned long long maxBufferSize;
        unsigned long maxVertexAttributes;
        unsigned long maxVertexBufferArrayStride;
        unsigned long maxInterStageShaderVariables;
        unsigned long maxColorAttachments;
        unsigned long maxColorAttachmentBytesPerSample;
        unsigned long maxComputeWorkgroupStorageSize;
        unsigned long maxComputeInvocationsPerWorkgroup;
        unsigned long maxComputeWorkgroupSizeX;
        unsigned long maxComputeWorkgroupSizeY;
        unsigned long maxComputeWorkgroupSizeZ;
        unsigned long maxComputeWorkgroupsPerDimension;
    };

    struct DeviceDescriptor {
        SupportedFeatures requiredFeatures;
        SupportedLimits requiredLimits;
        DeviceInfo info;
        bool enableDebug;
    };

    class Device {
    public:
		Device(DeviceDescriptor desc) : desc{desc} {}

		virtual std::shared_ptr<Buffer> createBuffer(BufferDescriptor desc) = 0;
		virtual std::shared_ptr<Texture> createTexture(TextureDescriptor desc) = 0;
		virtual std::shared_ptr<Sampler> createSampler(SamplerDescriptor desc) = 0;
		virtual std::shared_ptr<BindGroupLayout> createBindGroupLayout(BindGroupLayoutDescriptor desc) = 0;
        virtual std::shared_ptr<BindGroup> createBindGroup(BindGroupDescriptor desc) = 0;
		virtual std::shared_ptr<PipelineLayout> createPipelineLayout(PipelineLayoutDescriptor desc) = 0;
		virtual std::shared_ptr<ShaderModule> createShaderModule(ShaderModuleDescriptor desc) = 0;
		virtual std::shared_ptr<ComputePipeline> createComputePipeline(ComputePipelineDescriptor desc) = 0;
		virtual std::shared_ptr<RenderPipeline> createRenderPipeline(RenderPipelineDescriptor desc) = 0;

		CommandEncoder createCommandEncoder();

		QuerySet createQuerySet(QuerySetDescriptor desc);

    protected:
        DeviceDescriptor desc;
    };

    // ===========================================================================================================================
    // Adapter
    // ===========================================================================================================================

    class Adapter {
        virtual std::shared_ptr<Device> requestDevice() = 0;
    };
};

