#pragma once

#include "../rhi.hpp"

namespace RHI {
    // ===========================================================================================================================
    // Class Definition
    // ===========================================================================================================================

    enum class PipelineStage : FlagsConstant {
        eCompute            = 0x0001,
        eVertex             = 0x0002,
        eFragment           = 0x0004,
        eTessellCtrl        = 0x0008,
        eTessellEval        = 0x0010,
        eTask               = 0x0020,
        eMesh               = 0x0040,
        eTransfer           = 0x0080,
        eAttachmentOutput   = 0x0100,
        eEarlyFragmentTest  = 0x0200,
        eLateFragmentTest   = 0x0400,
        ePresent            = 0x0800
    };

    enum class TextureState : Uint8 {
        eUndefined,
        eColorAttachment,
        eDepthStencilAttachment,
        eColorTextureBinding,
        eDepthStencilTextureBinding,
        eStorageBinding,
        eCopySrc,
        eCopyDst,
        ePresent
    };
   
    struct BufferInfo {
        Buffer* buffer;
        Uint64 size;
        Uint64 offset;
    };

    struct BufferCommandState {
        BufferInfo target;
        PipelineStage stage;
        ResourceAccess access;
    };

    struct TextureCommandState {
        TextureView* target;
        TextureState state;
        PipelineStage stage;
        ResourceAccess access;
    };

    // ===========================================================================================================================
    // Barrier
    // ===========================================================================================================================

    struct BufferBarrierState {
        PipelineStage stage;
        ResourceAccess access;
        BufferInfo desc;
    };

    struct TextureBarrierState {
        PipelineStage stage;
        ResourceAccess access;
        TextureView* target;
    };

    class CommandBarrier {
    public:
        virtual void recordBufferBarrier(CommandBuffer* commandBuffer, BufferInfo target, PipelineStage stage, ResourceAccess access) = 0;

        virtual void recordTextureBarrier(CommandBuffer* commandBuffer, TextureView* target, TextureState state,
            PipelineStage stage, ResourceAccess access) = 0;

    private:
        std::vector<BufferBarrierState> curBufferStates;
        std::vector<TextureBarrierState> curTextureStates;
    };
    
    // ===========================================================================================================================
    // Command Item
    // ===========================================================================================================================

    class CommandItem {
    public:
        virtual std::vector<BufferCommandState> getBufferState() = 0;

        virtual std::vector<TextureCommandState> getTextureState() = 0;

        virtual void execute(CommandBuffer* commandBuffer) = 0;
    };

    class BeginRenderPassCommand : public CommandItem {
    public:
        BeginRenderPassCommand(
            RenderGraph* rg,
            Uint32 rpi,
            std::vector<TextureView*> ctv, 
            TextureView* dtv, 
            Extent3D sz
        )
        : renderGraph{rg},
          renderPassIndex{rpi},
          colorTextureViews{ctv},
          depthStencilTextureView{dtv},
          size{sz}
        {

        }

        std::vector<BufferCommandState> getBufferState() override;

        std::vector<TextureCommandState> getTextureState() override;

        void execute(CommandBuffer* commandBuffer) override {}

    protected:
        RenderGraph* renderGraph;
        Uint32 renderPassIndex;
        std::vector<TextureView*> colorTextureViews;
        TextureView* depthStencilTextureView;
        Extent3D size;
    };

    // ===========================================================================================================================
    // Command Encoder
    // ===========================================================================================================================

    class CommonCommandEncoder : public CommandEncoder {
    public:
        CommonCommandEncoder(
            std::shared_ptr<CommandBuffer> cb, 
            CommandBarrier* br
        ) 
        : commandBuffer{cb}, 
          barrier{br} 
        {}

        ~CommonCommandEncoder() {
            delete this->barrier;
        }

        virtual std::shared_ptr<RenderGraphCommandEncoder> startRenderGraph(RenderGraph* renderGraph) override {
            
        }

        std::shared_ptr<CommandBuffer> finish() override;

        std::vector<CommandItem*> getCommandList() { return this->commandList; }
        
    private:
        std::vector<CommandItem*> commandList;

        std::shared_ptr<CommandBuffer> commandBuffer;
        CommandBarrier* barrier;
    };

    class CommonRenderPassCommandEncoder : public RenderPassCommandEncoder {
    public:
        CommonRenderPassCommandEncoder(
            CommandEncoder* ce,
            RenderGraph* rg,
            Uint32 rpi
        )
        : commandEncoder{ce}, 
          renderGraph{rg},
          renderPassIndex{rpi}
        {

        }

    private:
        CommandEncoder* commandEncoder;
        RenderGraph* renderGraph;
        Uint32 renderPassIndex;
    };

    class CommonRenderGraphCommandEncoder : public RenderGraphCommandEncoder {
    public:
        CommonRenderGraphCommandEncoder(
            CommandEncoder* ce,
            RenderGraph* rg
        ) 
        : commandEncoder{ce}, 
          renderGraph{rg}
        {

        }

        std::shared_ptr<RenderPassCommandEncoder> beginRenderPass(Uint32 renderPassIndex, std::vector<TextureView*> colorTextureViews, 
            TextureView* depthStencilTextureView, Extent3D size) override
        {
            static_cast<CommonCommandEncoder*>(this->commandEncoder)->getCommandList().emplace_back(
                new BeginRenderPassCommand(this->renderGraph, renderPassIndex, colorTextureViews, 
                    depthStencilTextureView, size)
            );

            return std::make_shared<CommonRenderPassCommandEncoder>(this->commandEncoder, this->renderGraph, renderPassIndex);
        }

    private:
        CommandEncoder* commandEncoder;
        RenderGraph* renderGraph;
    };
}