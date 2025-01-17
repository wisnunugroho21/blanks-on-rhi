#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace Rhi {
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

    struct Color {
        float r;
        float g;
        float b;
        float a;
    };

    struct Extent3D {
        Uint32 width;
        Uint32 height = 1;
        Uint32 depth = 1;
    };

    struct Origin3D {
        Uint32 x = 0;
        Uint32 y = 0;
        Uint32 z = 0;
    };

    struct Viewport {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    struct ScissorRect {
        float x;
        float y;
        float width;
        float height;
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

    struct ActiveBufferMapping {
        void* data;
        Uint64 size = ULLONG_MAX;
        Uint64 offset = 0;
    };
    
    struct BufferDescriptor {
        Uint64 size;
        BufferUsageFlags usage;
        BufferLocation location;
    };

    class Buffer {
        BufferDescriptor desc;

        ActiveBufferMapping currentMapping;
        BufferMapState mapState;    

        virtual void* map(Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;
        virtual void unmap() = 0;

        virtual void flush(Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;
        virtual void invalidate(Uint64 size = ULLONG_MAX, Uint64 offset = 0) = 0;
    };

    // ===========================================================================================================================
    // Texture
    // ===========================================================================================================================

    enum class TextureState : Uint8 {
        eUndefined,
        eGeneral,
        eColorAttachment,
        eDepthStencilAttachment,
        eShaderReadOnly,
        eDepthStencilReadOnly,
        eCopySrc,
        eCopyDst,
        ePresent
    };

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
        eR32uint,
        eR32sint,
        eR32float,
        eRG16uint,
        eRG16sint,
        eRG16float,
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
        eRG11B10Ufloat,

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
        eStencil
    };

    enum class TextureUsage : FlagsConstant {
        eCopySrc          = 0x01,
        eCopyDst          = 0x02,
        eTextureBinding   = 0x04,
        eStorageBinding   = 0x08,
        eRenderAttachment = 0x10
    };

    struct TextureDescriptor {
        Extent3D size;

        Uint32 sliceLayersNum = 1;
        Uint32 mipLevelCount = 1;
        Uint32 sampleCount = 1;

        TextureDimension dimension = TextureDimension::e2D;
        TextureUsageFlags usage;
        TextureFormat format;
    };

    struct TextureSubresource {
        TextureAspect aspect = TextureAspect::eColor;

        Uint32 baseMipLevel = 0;
        Uint32 mipLevelCount = 1;

        Uint32 baseArrayLayer = 0;
        Uint32 arrayLayerCount = 1;
    };

    struct TextureViewDescriptor {
        TextureViewDimension dimension = TextureViewDimension::e2D;
        TextureSubresource subresource;
    };

    class Texture {
        TextureDescriptor desc;
        TextureState state;
        
        virtual std::shared_ptr<TextureView> createView(TextureViewDescriptor descriptor) = 0;
    };

    class TextureView {
        TextureViewDescriptor desc;
        Texture* texture;
    }; 

    // ===========================================================================================================================
    // Sampler
    // ===========================================================================================================================

    enum class AddressMode : Uint8 {
        eClampToEdge,
        eRepeat,
        eMirrorRepeat
    };

    enum FilterMode : Uint8 {
        eNearest,
        eLinear
    };

    enum MipmapFilterMode : Uint8 {
        eNearest,
        eLinear
    };

    enum CompareFunction : Uint8 {
        eNever,
        eLess,
        eEqual,
        eLessEqual,
        eGreater,
        eNotEqual,
        eGreaterEqual,
        eAlways,
    };

    struct SamplerDescriptor {
        AddressMode addressModeU = AddressMode::eClampToEdge;
        AddressMode addressModeV = AddressMode::eClampToEdge;
        AddressMode addressModeW = AddressMode::eClampToEdge;
        FilterMode magFilter = FilterMode::eNearest;
        FilterMode minFilter = FilterMode::eNearest;
        MipmapFilterMode mipmapFilter = MipmapFilterMode::eNearest;
        CompareFunction compare = CompareFunction::eNever;

        Float32 lodMinClamp = 0;
        Float32 lodMaxClamp = 32;
        Uint32 maxAnisotropy = 1;
    };

    class Sampler {
        SamplerDescriptor desc;

        bool isComparison;
        bool isFiltering;
    };

    // ===========================================================================================================================
    // Resource Binding
    // ===========================================================================================================================

    enum class ShaderStage : FlagsConstant {
        eCompute         = 0x0001,
        eVertex          = 0x0002,
        eFragment        = 0x0004,
        eTessellation    = 0x0008,
        eTask            = 0x0010,
        eMesh            = 0x0020,
    };

    enum class BufferBindingType : Uint8 {
        eUniform,
        eStorage,
        eReadOnlyStorage
    };

    enum class SamplerBindingType : Uint8 {
        eFiltering,
        eNonFiltering,
        eComparision
    };

    enum class TextureSampleType : Uint8 {
        eFloat,
        eUnfilterableFloat,
        eDepth,
        eSint,
        eUint
    };

    enum class ResourceAccess : Uint8 {
        eWriteOnly,
        eReadOnly,
        eReadWrite
    };

    struct BufferBindingLayout {
        BufferBindingType type = BufferBindingType::eUniform;
        bool hasDynamicOffset = false;
        Uint64 minBindingSize = 0;
    };

    struct SamplerBindingLayout {
        SamplerBindingType type = SamplerBindingType::eFiltering;
    };

    struct TextureBindingLayout {
        TextureSampleType sampleType = TextureSampleType::eFloat;
        TextureViewDimension viewDimension = TextureViewDimension::e2D;
        bool multisampled = false;
    };

    struct StorageTextureBindingLayout {
        ResourceAccess access = ResourceAccess::eWriteOnly;
        TextureViewDimension viewDimension = TextureViewDimension::e2D;
        TextureFormat format;
    };

    struct BindGroupLayoutEntry {
        Uint32 binding;
        ShaderStageFlags visibility;
    };

    struct BufferBindGroupLayoutEntry : BindGroupLayoutEntry {
        BufferBindingLayout buffer;
    };

    struct SamplerBindGroupLayoutEntry : BindGroupLayoutEntry {
        SamplerBindingLayout sampler;
    };

    struct TextureBindGroupLayoutEntry : BindGroupLayoutEntry {
        TextureBindingLayout texture;
    };

    struct StorageTextureBindGroupLayoutEntry : BindGroupLayoutEntry {
        StorageTextureBindingLayout storageTexture;
    };

    struct BindGroupLayoutDescriptor {
        std::vector<BindGroupLayoutEntry> entries;
    };

    class BindGroupLayout {
        BindGroupLayoutDescriptor desc;
    };

    struct BufferBinding {
        Buffer* buffer;
        Uint64 size = ULLONG_MAX;
        Uint64 offset = 0;
    };

    struct BindGroupEntry {
        Uint32 binding;
    };

    struct BufferBindGroupEntry : BindGroupEntry {
        BufferBinding resource;
    };

    struct TextureBindGroupEntry : BindGroupEntry {
        TextureView* resource;
    };

    struct SamplerBindGroupEntry : BindGroupEntry {
        Sampler* resource;
    };

    struct BindGroupDescriptor {
        BindGroupLayout* layout;
        std::vector<BindGroupEntry*> entries;
    };

    struct PipelineLayoutDescriptor {
        std::vector<BindGroupLayout*> bindGroupLayouts;
    };

    class BindGroup {
        BindGroupDescriptor desc;
    };

    class PipelineLayout {
        PipelineLayoutDescriptor desc;
    };

    // ===========================================================================================================================
    // Shader Module
    // ===========================================================================================================================

    enum class CompilationMessageType : Uint8 {
        Error,
        Warning,
        Info
    };

    struct CompilationMessage {
        String message;
        CompilationMessageType type;
        Uint64 lineNum;
        Uint64 linePos;
        Uint64 offset;
        Uint64 length;
    };

    struct CompilationInfo {
        CompilationMessage messages[];
    };

    struct ShaderModuleCompilationHint {
        String entryPoint;
        PipelineLayout layout;
    };

    struct ShaderModuleDescriptor {
        String code;
        std::vector<ShaderModuleCompilationHint> compilationHints{};
    };

    class ShaderModule {
        ShaderModuleDescriptor desc;

        virtual CompilationInfo getCompilationInfo() = 0;
    };

    // ===========================================================================================================================
    // Pipeline
    // ===========================================================================================================================

    enum class  PipelineErrorReason : Uint8 {
        eValidation,
        eInternal
    };

    enum VertexStepMode : Uint8 {
        eVertex,
        eInstance
    };

    enum VertexFormat : Uint8 {
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
        eFloat32x3,
        eFloat32x4,
        eUint32,
        eUint32x2,
        eUint32x3,
        eUint32x4,
        eSint32,
        eSint32x2,
        eSint32x3,
        eSint32x4,
        eUnorm1010102
    };

    enum PrimitiveTopology : Uint8 {
        ePointList,
        eLineList,
        eLineStrip,
        eTriangleList,
        eTriangleStrip
    };

    enum IndexFormat : Uint8 {
        eUint16,
        eUint32
    };

    enum FrontFace : Uint8 {
        eCCW,
        eCW
    };

    enum CullMode : Uint8 {
        eNone,
        eFront,
        eBack
    };

    enum PolygonMode : Uint8 {
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

    enum BlendOperation : Uint8 {
        eAdd,
        eSubtract,
        eReverseSubtract,
        eMin,
        eMax
    };

    enum BlendFactor : Uint8 {
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

    enum ColorWrite : FlagsConstant {
        eRED   = 0x1,
        eGREEN = 0x2,
        eBLUE  = 0x4,
        eALPHA = 0x8,
        eALL   = 0xF
    };

    struct PipelineError {
        PipelineErrorReason reason;
        const char* message;
    };

    struct ProgrammableStage {
        ShaderModule* module;
        const char* entryPoint;
        std::map<const char*, Float64> constants;
    };

    struct VertexAttribute {
        VertexFormat format;
        Uint64 offset;
        Uint32 shaderLocation;
    };

    struct BlendComponent {
        BlendOperation operation = BlendOperation::eAdd;
        BlendFactor srcFactor = BlendFactor::eOne;
        BlendFactor dstFactor = BlendFactor::eZero;
    };

    struct BlendState {
        BlendComponent color;
        BlendComponent alpha;
    };

    struct ColorTargetState {
        TextureFormat format;

        BlendState blend;
        ColorWriteFlags writeMask = 0xF;  // ColorWrite.ALL
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

    struct RasterizationState {
        FrontFace frontFace = FrontFace::eCCW;
        CullMode cullMode = CullMode::eNone;
        PolygonMode polygonMode = PolygonMode::eFill;

        // Requires "depth-clip-control" feature.
        bool unclippedDepth = false;
    };

    struct StencilFaceState {
        CompareFunction compare = CompareFunction::eAlways;
        StencilOperation failOp = StencilOperation::eKeep;
        StencilOperation depthFailOp = StencilOperation::eKeep;
        StencilOperation passOp = StencilOperation::eKeep;
    };

    struct DepthStencilState {
        TextureFormat format;

        bool depthWriteEnabled;
        CompareFunction depthCompare;

        StencilFaceState stencilFront{};
        StencilFaceState stencilBack{};

        Uint64 stencilReadMask = 0xFFFFFFFF;
        Uint64 stencilWriteMask = 0xFFFFFFFF;

        Int64 depthBias = 0;
        float depthBiasSlopeScale = 0;
        float depthBiasClamp = 0;
    };

    struct MultisampleState {
        Uint32 count = 1;
        Uint32 mask = 0xFFFFFFFF;
        bool alphaToCoverageEnabled = false;
    };

    struct FragmentState : ProgrammableStage {
        std::vector<ColorTargetState> targets;
    };

    struct PipelineDescriptorBase {
        PipelineLayout layout;
    };

    struct ComputePipelineDescriptor : PipelineDescriptorBase {
        ProgrammableStage compute;
    };

    struct RenderPipelineDescriptor : PipelineDescriptorBase {
        VertexState vertex;
        FragmentState fragment;
        DepthStencilState depthStencil;

        PrimitiveState primitive{};
        RasterizationState rasterizationState{};
        MultisampleState multisample{};
    };

    class PipelineBase {
        virtual BindGroupLayout getBindGroupLayout(uint32_t index) = 0;
    };

    class ComputePipeline : PipelineBase {
        ComputePipelineDescriptor desc;
    };

    class RenderPipeline : PipelineBase {
        RenderPipelineDescriptor desc;

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
        uint64_t offset = 0;
        uint32_t bytesPerRow;
        uint32_t rowsPerImage;
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
        Open,
        Locked,
        Ended
    };

    class CommandsMixin {
        CommandState state;
    };

    class CommandEncoder : CommandsMixin, BarrierCommandsMixin {
        virtual std::shared_ptr<RenderPassEncoder> beginRenderPass(RenderPassDescriptor descriptor) = 0;
        virtual std::shared_ptr<ComputePassEncoder> beginComputePass(ComputePassDescriptor descriptor) = 0;

        virtual void copyBufferToBuffer(
            Buffer source,
            Uint64 sourceOffset,
            Buffer destination,
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
            Buffer buffer,
            Uint64 offset = 0,
            Uint64 size) = 0;

        virtual void resolveQuerySet(
            QuerySet querySet,
            Uint32 firstQuery,
            Uint32 queryCount,
            Buffer destination,
            Uint64 destinationOffset) = 0;

        virtual void finish() = 0;
    };

    // ===========================================================================================================================
    // Programmable Passes
    // ===========================================================================================================================

    class BindingCommandsMixin {
        virtual void setBindGroup(Uint32 index, BindGroup bindGroup, std::vector<Uint32> dynamicOffsets) = 0;

        virtual void setBindGroup(Uint32 index, BindGroup bindGroup, Uint32 dynamicOffsetsData[],
            Uint64 dynamicOffsetsDataStart, Uint32 dynamicOffsetsDataLength) = 0;
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

    class ComputePassEncoder : CommandsMixin, BindingCommandsMixin {
        ComputePassDescriptor desc;
        CommandEncoder* commandEncoder;

        virtual void setPipeline(ComputePipeline* pipeline) = 0;
        virtual void dispatchWorkgroups(Uint32 workgroupCountX, Uint32 workgroupCountY = 1, Uint32 workgroupCountZ = 1) = 0;
        virtual void dispatchWorkgroupsIndirect(Buffer indirectBuffer, Uint64 indirectOffset) = 0;

        virtual void end() = 0;
    };

    // ===========================================================================================================================
    // Render Passes
    // ===========================================================================================================================

    enum class LoadOp : Uint8 {
        Load,
        Clear
    };

    enum StoreOp : Uint8 {
        Store,
        Discard
    };

    struct RenderPassColorAttachment {
        TextureView* view;
        TextureView* resolveTarget;

        Color clearValue;
        LoadOp loadOp;
        StoreOp storeOp;
    };

    struct RenderPassDepthStencilAttachment {
        TextureView* view;

        float depthClearValue;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        bool depthReadOnly = false;

        Uint32 stencilClearValue = 0;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        bool stencilReadOnly = false;
    };

    struct RenderPassTimestampWrites {
        QuerySet* querySet;
        Uint32 beginningOfPassWriteIndex;
        Uint32 endOfPassWriteIndex;
    };

    class RenderCommandsMixin {
        virtual void setPipeline(RenderPipeline pipeline) = 0;

        virtual void setIndexBuffer(Buffer buffer, IndexFormat indexFormat, Uint64 offset = 0, Uint64 size) = 0;
        virtual void setVertexBuffer(Uint32 slot, Buffer buffer, Uint64 offset = 0, Uint64 size) = 0;

        virtual void draw(Uint32 vertexCount, Uint32 instanceCount = 1, 
            Uint32 firstVertex = 0, Uint32 firstInstance = 0) = 0;
        virtual void drawIndexed(Uint32 indexCount, Uint32 instanceCount = 1,
            Uint32 firstIndex = 0, Int32 baseVertex = 0,
            Uint32 firstInstance = 0) = 0;

        virtual void drawIndirect(Buffer indirectBuffer, Uint64 indirectOffset) = 0;
        virtual void drawIndexedIndirect(Buffer indirectBuffer, Uint64 indirectOffset) = 0;
    };

    struct RenderPassDescriptor {
        std::vector<RenderPassColorAttachment> colorAttachments;
        RenderPassDepthStencilAttachment depthStencilAttachment;
        QuerySet* occlusionQuerySet;
        RenderPassTimestampWrites timestampWrites;
        Uint64 maxDrawCount = 50000000;
    };

    class RenderPassEncoder : CommandsMixin, BindingCommandsMixin, RenderCommandsMixin {
        RenderPassDescriptor desc;
        CommandEncoder* commandEncoder;

        virtual void setViewport(float x, float y,
                                float width, float height,
                                float minDepth, float maxDepth) = 0;

        virtual void setScissorRect(Uint32 x, Uint32 y,
                                    Uint32 width, Uint32 height) = 0;

        virtual void setBlendConstant(Color color) = 0;
        virtual void setStencilReference(Uint32 reference) = 0;

        virtual void beginOcclusionQuery(Uint32 queryIndex) = 0;
        virtual void endOcclusionQuery() = 0;

        virtual void end() = 0;
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
    };

    class Device {
        DeviceDescriptor desc;


    };

    // ===========================================================================================================================
    // Adapter
    // ===========================================================================================================================

    class Adapter {
        virtual std::shared_ptr<Device> requestDevice() = 0;
    };
};

