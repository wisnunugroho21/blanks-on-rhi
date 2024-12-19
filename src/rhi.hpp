#pragma onceRenderGraphDescriptor

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
    class RenderGraph;

    struct RenderGraphDescriptor;
    struct RenderPassDescriptor;
    struct RenderPipelineDescriptor;
    struct BindGroupDescriptor;

    struct RenderPassColorAttachment;
    struct RenderPassDepthStencilAttachment;
    struct RenderPassDescriptor;
    struct ComputePassDescriptor;

    enum class QueueType : uint8_t;

    // ===========================================================================================================================
    // Basic Type
    // ===========================================================================================================================
    
    typedef const char* String;
    typedef uint8_t Uint8;
    typedef uint16_t Uint16;
    typedef uint32_t Uint32;
    typedef uint64_t Uint64;

    typedef int16_t Int16;
    typedef int32_t Int32;
    typedef int64_t Int64;

    typedef float_t Float32;
    typedef double_t Float64;

    typedef uint32_t FlagsConstant;
    typedef uint32_t TextureUsageFlags;
    typedef uint32_t BufferUsageFlags;
    typedef uint32_t ShaderStageFlags;
    typedef uint32_t PipelineStageFlags;
    typedef uint32_t ColorWriteFlags;
    typedef uint32_t ResolveModeFlags;

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
    };

    struct Rect2D {
        Int32 x;
        Int32 y;
        Uint32 width;
        Uint32 height;
    };

    struct BaseDescriptor {
        String label = "";
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
        eUndefined = 0u,

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
    };

    struct TextureSubresource {
        TextureAspect aspect = TextureAspect::eColor;

        Uint32 baseMipLevel = 0;
        Uint32 mipLevelCount = 1;

        Uint32 baseArrayLayer = 0;
        Uint32 arrayLayerCount = 1;
    };

    struct TextureViewDescriptor {
        TextureSubresource subresource;
        TextureFormat format;

        TextureViewDimension dimension = TextureViewDimension::e2D;
    };

    class Texture {
    public:
        virtual TextureDescriptor getDesc() = 0;
        virtual TextureState getState() = 0;

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
        float maxAnisotropy = 1.0f;
    };

    class Sampler {
    public:
        virtual SamplerDescriptor getDesc() = 0;

        virtual bool isComparison() = 0;
        virtual bool isFiltering() = 0;
    };

    // ===========================================================================================================================
    // Bind Group
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

    enum class ResourceAccess : Uint8 {
        eWriteOnly,
        eReadOnly,
        eReadWrite
    };

    struct BindGroupDescriptorEntry {
        Uint32 binding;
        ShaderStageFlags shaderStage;
        BindingType type;

        Uint32 bindCount = 1;
    };

    struct BindGroupLayoutDescriptor : BaseDescriptor {
        std::vector<BindGroupDescriptorEntry> entries;
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

    struct VertexAttribute {
        VertexFormat format;
        Uint32 offset;
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
        Uint32 arrayStride;
        std::vector<VertexAttribute> attributes;

        VertexStepMode stepMode = VertexStepMode::eVertex;
    };

    struct VertexState {
        ShaderModule* module;
        std::vector<VertexBufferLayout> buffers;
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

    struct FragmentState {
        ShaderModule* module;
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

    struct RenderPipelineDescriptor : BaseDescriptor {
        std::vector<BindGroupLayoutDescriptor> bindGroupLayouts;

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

    // ===========================================================================================================================
    // Render Pass
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
        TextureFormat format = TextureFormat::eUndefined;
        Uint32 sampleCount = 1;

        Color clearValue = Color(0.0f);
        LoadOp loadOp = LoadOp::eClear;
        StoreOp storeOp = StoreOp::eStore;
    };

    struct RenderPassDepthStencilAttachment {
        TextureFormat format = TextureFormat::eUndefined;
        Uint32 sampleCount = 1;

        float depthClearValue = 1.0f;
        LoadOp depthLoadOp = LoadOp::eClear;
        StoreOp depthStoreOp = StoreOp::eDiscard;

        Uint32 stencilClearValue = 0u;
        LoadOp stencilLoadOp = LoadOp::eClear;
        StoreOp stencilStoreOp = StoreOp::eDiscard;
    };

    struct RenderPassDescriptor : BaseDescriptor {
        std::vector<RenderPipelineDescriptor> renderPipelines;

        std::vector<RenderPassColorAttachment> colorAttachments;
        RenderPassDepthStencilAttachment depthStencilAttachment;
    };

    // ===========================================================================================================================
    // Render Graph
    // ===========================================================================================================================

    struct RenderGraphDescriptor : BaseDescriptor {
        std::vector<RenderPassDescriptor> renderPasses;
    };

    class RenderGraph {
    public:
        virtual RenderGraphDescriptor getDesc() = 0;
    };

    // ===========================================================================================================================
    // Command Encoder
    // ===========================================================================================================================

    enum class CommandState : Uint8 {
        eOpen,
        eEnded,
        eSubmitted
    };

    struct CommandEncoderDescriptor {
        QueueType queueType;
    };

    struct CopyTexture {
        TextureView* view;
        Origin3D origin{};
    };

    struct CopyBuffer {
        Buffer* buffer;

        Uint64 offset = 0;
        Uint32 bytesPerRow;
        Uint32 rowsPerImage;
    };

    class CommandsMixin {
    public:
        virtual CommandState getCommandState() = 0;
    };

    class CommandEncoder : public CommandsMixin {
    public:
        virtual CommandEncoderDescriptor getDesc() = 0;

        virtual void copyBufferToBuffer(
            Buffer* source,
            Uint64 sourceOffset,
            Buffer* destination,
            Uint64 destinationOffset,
            Uint64 size
        ) = 0;

        virtual void copyBufferToTexture(
            CopyBuffer source,
            CopyTexture destination,
            Extent3D copySize
        ) = 0;

        virtual void copyTextureToBuffer(
            CopyTexture source,
            CopyBuffer destination,
            Extent3D copySize
        ) = 0;

        virtual void copyTextureToTexture(
            CopyTexture source,
            CopyTexture destination,
            Extent3D copySize
        ) = 0;

        virtual void fillBuffer(
            Uint32 data,
            Buffer* buffer,
            Uint64 size = ULLONG_MAX,
            Uint64 offset = 0
        ) = 0;

        virtual void finish() = 0;
    };

    // ===========================================================================================================================
    // Queue
    // ===========================================================================================================================

    enum class QueueType : uint8_t {
        Graphic,
        Compute,
        Transfer
    };

    struct QueueDescriptor {
        QueueType type;
        uint32_t index;
        uint32_t maxSubmission;
    };

    class QueueAsync {

    };

    class Queue {
    public:
        virtual QueueDescriptor getDesc() = 0;
        virtual void submit(std::vector<CommandEncoder*> commandBuffers) = 0;
    }; 

    // ===========================================================================================================================
    // Device
    // ===========================================================================================================================

    struct DeviceDescriptor {
        bool enableDebug;
    };

    class Device {
    public:
		Device(DeviceDescriptor desc) : desc{desc} {}

		virtual std::shared_ptr<Buffer> createBuffer(BufferDescriptor desc) = 0;
		virtual std::shared_ptr<Texture> createTexture(TextureDescriptor desc) = 0;
		virtual std::shared_ptr<Sampler> createSampler(SamplerDescriptor desc) = 0;
        virtual std::shared_ptr<RenderGraph> createRenderGraph(RenderGraphDescriptor desc) = 0;

		// QuerySet createQuerySet(QuerySetDescriptor desc);

    protected:
        DeviceDescriptor desc;
    };
};

