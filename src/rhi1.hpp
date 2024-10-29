#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

// ===========================================================================================================================
// Class Definition
// ===========================================================================================================================

class GPUBuffer;
class GPUTexture;
class GPUTextureView;
class GPUSampler;
class GPUBindGroupLayout;
class GPUBindGroup;
class GPUPipelineLayout;
class GPUShaderModule;
class GPUPipelineBase;
class GPUProgrammableStage;
class GPUQueue;
class GPUCommandBuffer;
class GPUCommandsMixin;
class GPURenderPassEncoder;
class GPUComputePassEncoder;
class GPURenderBundle;
class GPUQueue;
class GPUDevice;
class GPUAdapter;

struct GPURenderPassColorAttachment;
struct GPURenderPassDepthStencilAttachment;
struct GPURenderPassDescriptor;
struct GPUComputePassDescriptor;

// ===========================================================================================================================
// Basic Type
// ===========================================================================================================================

typedef unsigned long GPUBufferDynamicOffset;
typedef unsigned long GPUStencilValue;
typedef unsigned long GPUSampleMask;
typedef long GPUDepthBias;

typedef unsigned long long GPUSize64;
typedef unsigned long GPUIntegerCoordinate;
typedef unsigned long GPUIndex32;
typedef unsigned long GPUSize32;
typedef long GPUSignedOffset32;

typedef unsigned long long GPUSize64Out;
typedef unsigned long GPUIntegerCoordinateOut;
typedef unsigned long GPUSize32Out;

typedef unsigned long GPUFlagsConstant;
typedef unsigned long GPUTextureUsageFlags;
typedef unsigned long GPUBufferUsageFlags;
typedef unsigned long GPUMapModeFlags;
typedef unsigned long GPUShaderStageFlags;
typedef unsigned long GPUColorWriteFlags;
typedef double GPUPipelineConstantValue;

struct GPUOrigin3DDict {
    GPUIntegerCoordinate x = 0;
    GPUIntegerCoordinate y = 0;
    GPUIntegerCoordinate z = 0;
};

struct GPUExtent3DDict {
    GPUIntegerCoordinate width;
    GPUIntegerCoordinate height = 1;
    GPUIntegerCoordinate depthOrArrayLayers = 1;
};

typedef GPUOrigin3DDict GPUOrigin3D;
typedef GPUExtent3DDict GPUExtent3D;

struct GPUObjectDescriptorBase {
    const char* label = "";
};

// ===========================================================================================================================
// Buffer
// ===========================================================================================================================

enum class GPUBufferUsage : GPUFlagsConstant {
    MapRead         = 0x0001,
    MapWrite        = 0x0002,
    CopySrc         = 0x0004,
    CopyDst         = 0x0008,
    Index           = 0x0010,
    Vertex          = 0x0020,
    Uniform         = 0x0040,
    Storage         = 0x0080,
    Indirect        = 0x0100,
    QueryResolve    = 0x0200
};

enum class GPUMapMode : GPUFlagsConstant {
    Read  = 0x0001,
    Write = 0x0002
};

enum class GPUBufferMapState : uint8_t {
    Unmapped,
    Pending,
    Mapped
};

enum class GPUBufferInternalState : uint8_t {
    Available,
    UnAvailable,
    Destroyed
};

struct GPUActiveBufferMapping {
    void* data;
    GPUMapMode mode;
    GPUSize64 offset = 0; 
    GPUSize64 size;
};

struct GPUBufferDescriptor : GPUObjectDescriptorBase {
    GPUSize64 size;
    GPUBufferUsageFlags usage;
    bool mappedAtCreation = false;
};

class GPUBuffer {
    GPUBufferDescriptor desc;
    GPUActiveBufferMapping mapping;

    GPUBufferMapState mapState;
    GPUBufferInternalState internalState;    

    virtual void map(GPUMapModeFlags mode, GPUSize64 offset = 0, GPUSize64 size) = 0;
    virtual void* getMappedRange(GPUSize64 offset = 0, GPUSize64 size) = 0;
    virtual void unmap() = 0;

    virtual void destroy() = 0;
};

// ===========================================================================================================================
// Texture
// ===========================================================================================================================

enum GPUTextureFormat : uint8_t {
    // 8-bit formats
    R8_UNORM,
    R8_SNORM,
    R8_UINT,
    R8_SINT,

    // 16-bit formats
    R16_UINT,
    R16_SINT,
    R16_FLOAT,
    RG8_UNORM,
    RG8_SNORM,
    RG8_UINT,
    RG8_SINT,

    // 32-bit formats
    R32_UINT,
    R32_SINT,
    R32_FLOAT,
    RG16_UINT,
    RG16_SINT,
    RG16_FLOAT,
    RGBA8_UNORM,
    RGBA8_UNORM_SRGB,
    RGBA8_SNORM,
    RGBA8_UINT,
    RGBA8_SINT,
    BGRA8_UNORM,
    BGRA8_UNORM_SRGB,

    // Packed 32-bit formats
    RGB9E5_UFLOAT,
    RGB10A2_UINT,
    RGB10A2_UNORM,
    RG11B10_UFLOAT,

    // 64-bit formats
    RG32_UINT,
    RG32_SINT,
    RG32_FLOAT,
    RGBA16_UINT,
    RGBA16_SINT,
    RGBA16_FLOAT,

    // 128-bit formats
    RGBA32_UINT,
    RGBA32_SINT,
    RGBA32_FLOAT,

    // Depth/stencil formats
    S8_UINT,
    D16_UNORM,
    D24_PLUS,
    D24_PLUS_S8_UINT,
    D32_SFLOAT,

    // "depth32float-stencil8" feature
    D32_SFLOAT_S8_UINT,

    // BC compressed formats usable if "texture-compression-bc" is both
    // supported by the device/user agent and enabled in requestDevice.
    BC1_RGBA_UNORM,
    BC1_RGBA_UNORM_SRGB,
    BC2_RGBA_UNORM,
    BC2_RGBA_UNORM_SRGB,
    BC3_RGBA_UNORM,
    BC3_RGBA_UNORM_SRGB,
    BC4_R_UNORM,
    BC4_R_SNORM,
    BC5_RG_UNORM,
    BC5_RG_SNORM,
    BC6H_RGB_UFLOAT,
    BC6H_RGB_SFLOAT,
    BC7_RGBA_UNORM,
    BC7_RGBA_UNORM_SRGB,

    // ETC2 compressed formats usable if "texture-compression-etc2" is both
    // supported by the device/user agent and enabled in requestDevice.
    ETC2_RGB8_UNORM,
    ETC2_RGB8_UNORM_SRGB,
    ETC2_RGB8A1_UNORM,
    ETC2_RGB8A1_UNORM_SRGB,
    ETC2_RGBA8_UNORM,
    ETC2_RGBA8_UNORM_SRGB,
    EAC_R11_UNORM,
    EAC_R11_SNORM,
    EAC_RG11_UNORM,
    EAC_RG11_SNORM,

    // ASTC compressed formats usable if "texture-compression-astc" is both
    // supported by the device/user agent and enabled in requestDevice.
    ASTC_4X4_UNORM,
    ASTC_4X4_UNORM_SRGB,
    ASTC_5X4_UNORM,
    ASTC_5X4_UNORM_SRGB,
    ASTC_5X5_UNORM,
    ASTC_5X5_UNORM_SRGB,
    ASTC_6X5_UNORM,
    ASTC_6X5_UNORM_SRGB,
    ASTC_6X6_UNORM,
    ASTC_6X6_UNORM_SRGB,
    ASTC_8X5_UNORM,
    ASTC_8X5_UNORM_SRGB,
    ASTC_8X6_UNORM,
    ASTC_8X6_UNORM_SRGB,
    ASTC_8X8_UNORM,
    ASTC_8X8_UNORM_SRGB,
    ASTC_10X5_UNORM,
    ASTC_10X5_UNORM_SRGB,
    ASTC_10X6_UNORM,
    ASTC_10X6_UNORM_SRGB,
    ASTC_10X8_UNORM,
    ASTC_10X8_UNORM_SRGB,
    ASTC_10X10_UNORM,
    ASTC_10X10_UNORM_SRGB,
    ASTC_12X10_UNORM,
    ASTC_12X10_UNORM_SRGB,    
    ASTC_12X12_UNORM,
    ASTC_12X12_UNORM_SRGB
};

enum class GPUTextureDimension : uint8_t {
    D1,
    D2,
    D3
};

enum class GPUTextureViewDimension : uint8_t {
    D1,
    D2,
    D2Array,
    Cube,
    CubeArray,
    D3
};

enum class GPUTextureAspect : uint8_t {
    All,
    StencilOnly,
    DepthOnly
};

enum class GPUTextureUsage : GPUFlagsConstant {
    CopySrc          = 0x01,
    SopyDst          = 0x02,
    TextureBinding   = 0x04,
    StorageBinding   = 0x08,
    RenderAttachment = 0x10
};

struct GPUTextureDescriptor : GPUObjectDescriptorBase {
    GPUExtent3D size;
    GPUIntegerCoordinate mipLevelCount = 1;
    GPUSize32 sampleCount = 1;
    GPUTextureDimension dimension = GPUTextureDimension::D2;
    GPUTextureFormat format;
    GPUTextureUsageFlags usage;
    std::vector<GPUTextureFormat> viewFormats{};
};

struct GPUTextureViewDescriptor {
    GPUTextureFormat format;
    GPUTextureViewDimension dimension;
    GPUTextureUsageFlags usage = 0;
    GPUTextureAspect aspect = GPUTextureAspect::All;
    GPUIntegerCoordinate baseMipLevel = 0;
    GPUIntegerCoordinate mipLevelCount;
    GPUIntegerCoordinate baseArrayLayer = 0;
    GPUIntegerCoordinate arrayLayerCount;
};

class GPUTexture {
    GPUTextureDescriptor desc;
    bool isDestroyed;

    virtual std::shared_ptr<GPUTextureView> createView(GPUTextureViewDescriptor descriptor = {}) = 0;

    virtual void destroy() = 0;
};

class GPUTextureView {
    GPUTextureViewDescriptor desc;
    bool isDestroyed;

    GPUTexture* texture;
    GPUExtent3DDict renderExtent;

    virtual void destroy() = 0;
};

// ===========================================================================================================================
// Sampler
// ===========================================================================================================================

enum class GPUAddressMode : uint8_t {
    ClampToEdge,
    Repeat,
    MirrorRepeat
};

enum GPUFilterMode : uint8_t {
    Nearest,
    Linear
};

enum GPUMipmapFilterMode : uint8_t {
    Nearest,
    Linear
};

enum GPUCompareFunction : uint8_t {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

struct GPUSamplerDescriptor : GPUObjectDescriptorBase {
    GPUAddressMode addressModeU = GPUAddressMode::ClampToEdge;
    GPUAddressMode addressModeV = GPUAddressMode::ClampToEdge;
    GPUAddressMode addressModeW = GPUAddressMode::ClampToEdge;
    GPUFilterMode magFilter = GPUFilterMode::Nearest;
    GPUFilterMode minFilter = GPUFilterMode::Nearest;
    GPUMipmapFilterMode mipmapFilter = GPUMipmapFilterMode::Nearest;
    float lodMinClamp = 0;
    float lodMaxClamp = 32;
    GPUCompareFunction compare;
    unsigned short maxAnisotropy = 1;
};

class GPUSampler {
    GPUSamplerDescriptor desc;
    bool isDestroyed;

    bool isComparison;
    bool isFiltering;

    virtual void destroy() = 0;
};

// ===========================================================================================================================
// Resource Binding
// ===========================================================================================================================

enum class GPUShaderStage : GPUFlagsConstant {
    Compute         = 0x0001,
    Vertex          = 0x0002,
    Fragment        = 0x0004,
    Tessellation    = 0x0008,
    Task            = 0x0010,
    Mesh            = 0x0020,
};

enum class GPUBufferBindingType : uint8_t {
    Uniform,
    Storage,
    ReadOnlyStorage
};

enum class GPUSamplerBindingType : uint8_t {
    Filtering,
    NonFiltering,
    Comparision
};

enum class GPUTextureSampleType : uint8_t {
    Float,
    UnfilterableFloat,
    Depth,
    Sint,
    Uint
};

enum class GPUStorageTextureAccess : uint8_t {
    WriteOnly,
    ReadOnly,
    ReadWrite
};

struct GPUBufferBindingLayout {
    GPUBufferBindingType type = GPUBufferBindingType::Uniform;
    bool hasDynamicOffset = false;
    GPUSize64 minBindingSize = 0;
};

struct GPUSamplerBindingLayout {
    GPUSamplerBindingType type = GPUSamplerBindingType::Filtering;
};

struct GPUTextureBindingLayout {
    GPUTextureSampleType sampleType = GPUTextureSampleType::Float;
    GPUTextureViewDimension viewDimension = GPUTextureViewDimension::D2;
    bool multisampled = false;
};

struct GPUStorageTextureBindingLayout {
    GPUStorageTextureAccess access = GPUStorageTextureAccess::WriteOnly;
    GPUTextureFormat format;
    GPUTextureViewDimension viewDimension = GPUTextureViewDimension::D2;
};

struct GPUBindGroupLayoutEntry {
    GPUIndex32 binding;
    GPUShaderStageFlags visibility;

    GPUBufferBindingLayout buffer;
    GPUSamplerBindingLayout sampler;
    GPUTextureBindingLayout texture;
    GPUStorageTextureBindingLayout storageTexture;
};

struct GPUBindGroupLayoutDescriptor : GPUObjectDescriptorBase {
    std::vector<GPUBindGroupLayoutEntry> entries;
};

class GPUBindGroupLayout {
    GPUBindGroupLayoutDescriptor desc;
    bool isDestroyed;

    virtual void destroy() = 0;
};

struct GPUBufferBinding {
    GPUBuffer* buffer;
    GPUSize64 offset = 0;
    GPUSize64 size;
};

struct GPUBindGroupEntry {
    GPUIndex32 binding;
};

struct GPUBufferBindGroupEntry : GPUBindGroupEntry {
    GPUBufferBinding resource;
};

struct GPUTextureBindGroupEntry : GPUBindGroupEntry {
    GPUTextureView* resource;
};

struct GPUSamplerBindGroupEntry : GPUBindGroupEntry {
    GPUSampler* resource;
};

struct GPUBindGroupDescriptor : GPUObjectDescriptorBase {
    GPUBindGroupLayout *layout;
    std::vector<GPUBindGroupEntry*> entries;
};

class GPUBindGroup {
    GPUBindGroupDescriptor desc;
};

struct GPUPipelineLayoutDescriptor : GPUObjectDescriptorBase {
    std::vector<GPUBindGroupLayout*> bindGroupLayouts;
};

class GPUPipelineLayout {
    GPUPipelineLayoutDescriptor desc;
};

// ===========================================================================================================================
// Shader Module
// ===========================================================================================================================

enum class GPUCompilationMessageType : uint8_t {
    Error,
    Warning,
    Info
};

struct GPUCompilationMessage {
    const char* message;
    GPUCompilationMessageType type;
    unsigned long long lineNum;
    unsigned long long linePos;
    unsigned long long offset;
    unsigned long long length;
};

struct GPUCompilationInfo {
    std::vector<GPUCompilationMessage> messages;
};

struct GPUShaderModuleCompilationHint {
    const char* entryPoint;
    GPUPipelineLayout layout;
};

struct GPUShaderModuleDescriptor : GPUObjectDescriptorBase {
    const char* code;
    std::vector<GPUShaderModuleCompilationHint> compilationHints{};
};

class GPUShaderModule {
    GPUShaderModuleDescriptor desc;
    bool isDestroyed;

    virtual GPUCompilationInfo getCompilationInfo() = 0;

    virtual void destroy() = 0;
};

// ===========================================================================================================================
// Pipeline
// ===========================================================================================================================

enum class  GPUPipelineErrorReason : uint8_t {
    Validation,
    Internal
};

enum GPUVertexStepMode : uint8_t {
    Vertex,
    Instance
};

enum GPUVertexFormat : uint8_t {
    Uint8x2,
    Uint8x4,
    Sint8x2,
    Sint8x4,
    Unorm8x2,
    Unorm8x4,
    Snorm8x2,
    Snorm8x4,
    Uint16x2,
    Uint16x4,
    Sint16x2,
    Sint16x4,
    Unorm16x2,
    Unorm16x4,
    Snorm16x2,
    Snorm16x4,
    Float16x2,
    Float16x4,
    Float32,
    Float32x2,
    Float32x3,
    Float32x4,
    Uint32,
    Uint32x2,
    Uint32x3,
    Uint32x4,
    Sint32,
    Sint32x2,
    Sint32x3,
    Sint32x4,
    Unorm1010102
};

enum GPUPrimitiveTopology : uint8_t {
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip
};

enum GPUIndexFormat : uint8_t {
    Uint16,
    Uint32
};

enum GPUFrontFace : uint8_t {
    CCW,
    CW
};

enum GPUCullMode : uint8_t {
    None,
    Front,
    Back
};

enum GPUStencilOperation : uint8_t {
    Keep,
    Zero,
    Replace,
    Invert,
    IncrementClamp,
    DecrementClamp
};

enum GPUBlendOperation : uint8_t {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

enum GPUBlendFactor : uint8_t {
    Zero,
    One,
    Src,
    OneMinusSrc,
    SrcAlpha,
    OneMinusSrcAlpha,
    Dst,
    OneMinusDst,
    DstAlpha,
    OneMinusDstAlpha,
    SrcAlphaSaturated,
    Constant,
    OneMinusConstant,
    Src1,
    OneMinusSrc1,
    Src1Alpha,
    OneMinusSrc1Alpha,
};

enum GPUColorWrite : GPUFlagsConstant {
    RED   = 0x1,
    GREEN = 0x2,
    BLUE  = 0x4,
    ALPHA = 0x8,
    ALL   = 0xF
};

struct GPUPipelineError {
    GPUPipelineErrorReason reason;
    const char* message;
};

struct GPUProgrammableStage {
    GPUShaderModule* module;
    const char* entryPoint;
    std::map<const char*, GPUPipelineConstantValue> constants;
};

struct GPUVertexAttribute {
    GPUVertexFormat format;
    GPUSize64 offset;

    GPUIndex32 shaderLocation;
};

struct GPUBlendComponent {
    GPUBlendOperation operation = GPUBlendOperation::Add;
    GPUBlendFactor srcFactor = GPUBlendFactor::One;
    GPUBlendFactor dstFactor = GPUBlendFactor::Zero;
};

struct GPUBlendState {
    GPUBlendComponent color;
    GPUBlendComponent alpha;
};

struct GPUColorTargetState {
    GPUTextureFormat format;

    GPUBlendState blend;
    GPUColorWriteFlags writeMask = 0xF;  // GPUColorWrite.ALL
};

struct GPUVertexBufferLayout {
    GPUSize64 arrayStride;
    GPUVertexStepMode stepMode = GPUVertexStepMode::Vertex;
    std::vector<GPUVertexAttribute> attributes;
};

struct GPUVertexState : GPUProgrammableStage {
    std::vector<GPUVertexBufferLayout> buffers{};
};

struct GPUPrimitiveState {
    GPUPrimitiveTopology topology = GPUPrimitiveTopology::TriangleList;
    GPUIndexFormat stripIndexFormat;
    GPUFrontFace frontFace = GPUFrontFace::CCW;
    GPUCullMode cullMode = GPUCullMode::None;

    // Requires "depth-clip-control" feature.
    bool unclippedDepth = false;
};

struct GPUStencilFaceState {
    GPUCompareFunction compare = GPUCompareFunction::Always;
    GPUStencilOperation failOp = GPUStencilOperation::Keep;
    GPUStencilOperation depthFailOp = GPUStencilOperation::Keep;
    GPUStencilOperation passOp = GPUStencilOperation::Keep;
};

struct GPUDepthStencilState {
    GPUTextureFormat format;

    bool depthWriteEnabled;
    GPUCompareFunction depthCompare;

    GPUStencilFaceState stencilFront = {};
    GPUStencilFaceState stencilBack = {};

    GPUStencilValue stencilReadMask = 0xFFFFFFFF;
    GPUStencilValue stencilWriteMask = 0xFFFFFFFF;

    GPUDepthBias depthBias = 0;
    float depthBiasSlopeScale = 0;
    float depthBiasClamp = 0;
};

struct GPUMultisampleState {
    GPUSize32 count = 1;
    GPUSampleMask mask = 0xFFFFFFFF;
    bool alphaToCoverageEnabled = false;
};

struct GPUFragmentState : GPUProgrammableStage {
    std::vector<GPUColorTargetState> targets;
};

struct GPUPipelineDescriptorBase : GPUObjectDescriptorBase {
    GPUPipelineLayout layout;
};

struct GPUComputePipelineDescriptor : GPUPipelineDescriptorBase {
    GPUProgrammableStage compute;
};

struct GPURenderPipelineDescriptor : GPUPipelineDescriptorBase {
    GPUVertexState vertex;
    GPUPrimitiveState primitive = {};
    GPUDepthStencilState depthStencil;
    GPUMultisampleState multisample = {};
    GPUFragmentState fragment;
};

class GPUPipelineBase {
    GPUPipelineDescriptorBase desc;
    virtual GPUBindGroupLayout getBindGroupLayout(uint32_t index) = 0;
};

class GPUComputePipeline : GPUPipelineBase {
    GPUComputePipelineDescriptor desc;
};

class GPURenderPipeline : GPUPipelineBase {
    GPURenderPipelineDescriptor desc;

    bool writesDepth;
    bool writesStencil;
};

// ===========================================================================================================================
// Query
// ===========================================================================================================================

enum class GPUQueryType : uint8_t {
    Occulusion,
    Timestamp
};

struct GPUQuerySetDescriptor : GPUObjectDescriptorBase {
    GPUQueryType type;
    GPUSize32 count;
};

class GPUQuerySet {    
    GPUQuerySetDescriptor desc;
    bool isDestroyed;

    virtual void destroy() = 0;
};

// ===========================================================================================================================
// Debug Markers
// ===========================================================================================================================

struct GPUDebugCommandsMixin {
    virtual void pushDebugGroup(const char* groupLabel) = 0;
    virtual void popDebugGroup() = 0;
    virtual void insertDebugMarker(const char* markerLabel) = 0;
};

// ===========================================================================================================================
// Command Buffer
// ===========================================================================================================================

struct GPUViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};

struct GPUScissorRect {
    float x;
    float y;
    float width;
    float height;
};

struct GPUColor {
    double r;
    double g;
    double b;
    double a;
};

struct GPURenderState {
    GPUSize32 occlusionQueryIndex;
    GPUViewport viewport;
    GPUScissorRect scissorRect;
    GPUColor blendConstant;
    GPUStencilValue stencilReference;
    std::vector<GPURenderPassColorAttachment> colorAttachments;
    std::vector<GPURenderPassDepthStencilAttachment> depthStencilAttachment;
};

struct GPUCommandBufferDescriptor : GPUObjectDescriptorBase {

};

class GPUCommandBuffer {
    GPUCommandBufferDescriptor desc;
    GPURenderState renderState;
};

// ===========================================================================================================================
// Programmable Passes
// ===========================================================================================================================

class GPUBindingCommandsMixin {
    virtual void setBindGroup(GPUIndex32 index, GPUBindGroup bindGroup, std::vector<GPUBufferDynamicOffset> dynamicOffsets) = 0;

    virtual void setBindGroup(GPUIndex32 index, GPUBindGroup bindGroup, uint32_t dynamicOffsetsData[],
        GPUSize64 dynamicOffsetsDataStart, GPUSize32 dynamicOffsetsDataLength) = 0;
};

// ===========================================================================================================================
// Copies
// ===========================================================================================================================

struct GPUImageDataLayout {
    uint64_t offset = 0;
    uint32_t bytesPerRow;
    uint32_t rowsPerImage;
};

struct GPUImageCopyTexture {
    GPUTexture* texture;
    GPUIntegerCoordinate mipLevel = 0;
    GPUOrigin3D origin = {};
    GPUTextureAspect aspect = GPUTextureAspect::All;
};

struct GPUImageCopyBuffer : GPUImageDataLayout {
    GPUBuffer* buffer;
};

// ===========================================================================================================================
// Command Encoder
// ===========================================================================================================================

enum class GPUCommandState : uint8_t {
    Open,
    Locked,
    Ended
};

struct  GPUCommandEncoderDescriptor : GPUObjectDescriptorBase {
    
};

class GPUCommandsMixin {
    GPUCommandState state;
};

class GPUCommandEncoder : GPUCommandsMixin, GPUDebugCommandsMixin {
    GPUCommandEncoderDescriptor desc;

    virtual GPURenderPassEncoder beginRenderPass(GPURenderPassDescriptor descriptor) = 0;
    virtual GPUComputePassEncoder beginComputePass(GPUComputePassDescriptor descriptor) = 0;

    virtual void copyBufferToBuffer(
        GPUBuffer source,
        GPUSize64 sourceOffset,
        GPUBuffer destination,
        GPUSize64 destinationOffset,
        GPUSize64 size) = 0;

    virtual void copyBufferToTexture(
        GPUImageCopyBuffer source,
        GPUImageCopyTexture destination,
        GPUExtent3D copySize) = 0;

    virtual void copyTextureToBuffer(
        GPUImageCopyTexture source,
        GPUImageCopyBuffer destination,
        GPUExtent3D copySize) = 0;

    virtual void copyTextureToTexture(
        GPUImageCopyTexture source,
        GPUImageCopyTexture destination,
        GPUExtent3D copySize) = 0;

    virtual void clearBuffer(
        GPUBuffer buffer,
        GPUSize64 offset = 0,
        GPUSize64 size) = 0;

    virtual void resolveQuerySet(
        GPUQuerySet querySet,
        GPUSize32 firstQuery,
        GPUSize32 queryCount,
        GPUBuffer destination,
        GPUSize64 destinationOffset) = 0;

    virtual GPUCommandBuffer finish(GPUCommandBufferDescriptor descriptor) = 0;
};

// ===========================================================================================================================
// Compute Passes
// ===========================================================================================================================

struct GPUComputePassTimestampWrites {
    GPUQuerySet* querySet;
    GPUSize32 beginningOfPassWriteIndex;
    GPUSize32 endOfPassWriteIndex;
};

struct GPUComputePassDescriptor : GPUObjectDescriptorBase {
    GPUComputePassTimestampWrites timestampWrites;
};

class GPUComputePassEncoder : GPUCommandsMixin, GPUDebugCommandsMixin, GPUBindingCommandsMixin {
    GPUComputePassDescriptor desc;

    virtual void setPipeline(GPUComputePipeline pipeline) = 0;
    virtual void dispatchWorkgroups(GPUSize32 workgroupCountX, GPUSize32 workgroupCountY = 1, GPUSize32 workgroupCountZ = 1) = 0;
    virtual void dispatchWorkgroupsIndirect(GPUBuffer indirectBuffer, GPUSize64 indirectOffset) = 0;

    virtual void end() = 0;
};

// ===========================================================================================================================
// Render Passes
// ===========================================================================================================================

enum class GPULoadOp : uint8_t {
    Load,
    Clear
};

enum GPUStoreOp : uint8_t {
    Store,
    Discard
};

struct GPURenderPassLayout {
    std::vector<GPUTextureFormat> colorFormats;
    GPUTextureFormat depthStencilFormat;
    GPUSize32 sampleCount = 1;
};

struct GPURenderPassColorAttachment {
    GPUTextureView* view;
    GPUIntegerCoordinate depthSlice;
    GPUTextureView* resolveTarget;

    GPUColor clearValue;
    GPULoadOp loadOp;
    GPUStoreOp storeOp;
};

struct GPURenderPassDepthStencilAttachment {
    GPUTextureView* view;

    float depthClearValue;
    GPULoadOp depthLoadOp;
    GPUStoreOp depthStoreOp;
    bool depthReadOnly = false;

    GPUStencilValue stencilClearValue = 0;
    GPULoadOp stencilLoadOp;
    GPUStoreOp stencilStoreOp;
    bool stencilReadOnly = false;
};

struct GPURenderPassTimestampWrites {
    GPUQuerySet* querySet;
    GPUSize32 beginningOfPassWriteIndex;
    GPUSize32 endOfPassWriteIndex;
};

class GPURenderCommandsMixin {
    virtual void setPipeline(GPURenderPipeline pipeline) = 0;

    virtual void setIndexBuffer(GPUBuffer buffer, GPUIndexFormat indexFormat, GPUSize64 offset = 0, GPUSize64 size) = 0;
    virtual void setVertexBuffer(GPUIndex32 slot, GPUBuffer buffer, GPUSize64 offset = 0, GPUSize64 size) = 0;

    virtual void draw(GPUSize32 vertexCount, GPUSize32 instanceCount = 1, 
        GPUSize32 firstVertex = 0, GPUSize32 firstInstance = 0) = 0;
    virtual void drawIndexed(GPUSize32 indexCount, GPUSize32 instanceCount = 1,
        GPUSize32 firstIndex = 0, GPUSignedOffset32 baseVertex = 0,
        GPUSize32 firstInstance = 0) = 0;

    virtual void drawIndirect(GPUBuffer indirectBuffer, GPUSize64 indirectOffset) = 0;
    virtual void drawIndexedIndirect(GPUBuffer indirectBuffer, GPUSize64 indirectOffset) = 0;
};

struct GPURenderPassDescriptor : GPUObjectDescriptorBase {
    std::vector<GPURenderPassColorAttachment> colorAttachments;
    GPURenderPassDepthStencilAttachment depthStencilAttachment;
    GPUQuerySet* occlusionQuerySet;
    GPURenderPassTimestampWrites timestampWrites;
    GPUSize64 maxDrawCount = 50000000;
};

class GPURenderPassEncoder : GPUCommandsMixin, GPUDebugCommandsMixin, GPUBindingCommandsMixin, GPURenderCommandsMixin {
    GPUCommandEncoder* commandEncoder;
    GPURenderState state;

    virtual void setViewport(float x, float y,
                             float width, float height,
                             float minDepth, float maxDepth) = 0;

    virtual void setScissorRect(GPUIntegerCoordinate x, GPUIntegerCoordinate y,
                                GPUIntegerCoordinate width, GPUIntegerCoordinate height) = 0;

    virtual void setBlendConstant(GPUColor color) = 0;
    virtual void setStencilReference(GPUStencilValue reference) = 0;

    virtual void beginOcclusionQuery(GPUSize32 queryIndex) = 0;
    virtual void endOcclusionQuery() = 0;

    virtual void executeBundles(std::vector<GPURenderBundle> bundles) = 0;
    virtual void end() = 0;
};

// ===========================================================================================================================
// Bundle
// ===========================================================================================================================

struct GPURenderBundleDescriptor : GPUObjectDescriptorBase {

};

struct GPURenderBundleEncoderDescriptor : GPURenderPassLayout {
    bool depthReadOnly = false;
    bool stencilReadOnly = false;
};

class GPURenderBundle {
    GPUCommandBuffer *commandBuffer;
    GPURenderPassLayout layout;
    bool depthReadOnly;
    bool stencilReadOnly;
    GPUSize64 drawCount;
};

class GPURenderBundleEncoder : GPUCommandsMixin, GPUDebugCommandsMixin, GPUBindingCommandsMixin, GPURenderCommandsMixin {
    GPURenderBundleEncoderDescriptor desc;
    virtual GPURenderBundle finish(GPURenderBundleDescriptor descriptor) = 0;
};

// ===========================================================================================================================
// Queue
// ===========================================================================================================================

struct GPUQueueDescriptor : GPUObjectDescriptorBase {

};

class GPUQueue {
    GPUQueueDescriptor desc;
    virtual void submit(std::vector<GPUCommandBuffer*> commandBuffers) = 0;

    virtual void writeBuffer(
        GPUBuffer* buffer,
        GPUSize64 bufferOffset,
        void* data,
        GPUSize64 dataOffset = 0,
        GPUSize64 size
    ) = 0;

    virtual void writeTexture(
        GPUImageCopyTexture destination,
        void* data,
        GPUImageDataLayout dataLayout,
        GPUExtent3D size
    ) = 0;
};

// ===========================================================================================================================
// Device
// ===========================================================================================================================

struct GPUSupportedFeatures {
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

struct GPUSupportedLimits {
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

struct GPUDeviceDescriptor {
    GPUSupportedFeatures requiredFeatures;
    GPUSupportedLimits requiredLimits;
};

class GPUDevice {
    GPUDeviceDescriptor desc;
    GPUQueue* queue;

    virtual void destroy() = 0;

    GPUBuffer createBuffer(GPUBufferDescriptor descriptor);
    GPUTexture createTexture(GPUTextureDescriptor descriptor);
    GPUSampler createSampler(GPUSamplerDescriptor descriptor = {});

    GPUBindGroupLayout createBindGroupLayout(GPUBindGroupLayoutDescriptor descriptor);
    GPUPipelineLayout createPipelineLayout(GPUPipelineLayoutDescriptor descriptor);
    GPUBindGroup createBindGroup(GPUBindGroupDescriptor descriptor);

    GPUShaderModule createShaderModule(GPUShaderModuleDescriptor descriptor);
    GPUComputePipeline createComputePipeline(GPUComputePipelineDescriptor descriptor);
    GPURenderPipeline createRenderPipeline(GPURenderPipelineDescriptor descriptor);
    GPUComputePipeline createComputePipelineAsync(GPUComputePipelineDescriptor descriptor);
    GPURenderPipeline createRenderPipelineAsync(GPURenderPipelineDescriptor descriptor);

    GPUCommandEncoder createCommandEncoder(GPUCommandEncoderDescriptor descriptor = {});
    GPURenderBundleEncoder createRenderBundleEncoder(GPURenderBundleEncoderDescriptor descriptor);

    GPUQuerySet createQuerySet(GPUQuerySetDescriptor descriptor);

};

// ===========================================================================================================================
// Adapter
// ===========================================================================================================================

struct GPUAdapterInfo {
    const char* vendor;
    const char* architecture;
    const char* device;
    const char* description;
};

struct GPUAdapterDescriptor {
    GPUSupportedFeatures features;
    GPUSupportedLimits limits;
    GPUAdapterInfo info;
};

class GPUAdapter {
    GPUAdapterDescriptor desc;

    virtual std::shared_ptr<GPUDevice> requestDevice(GPUDeviceDescriptor desc) = 0;
};