#include "command.hpp"

namespace RHI {
    std::shared_ptr<CommandBuffer> CommonCommandEncoder::finish() {
        for (auto &&command : this->commandList) {
            for (auto &&bufferState : command->getBufferState()) {
                this->barrier->recordBufferBarrier(this->commandBuffer.get(), bufferState.target, 
                    bufferState.stage, bufferState.access);
            }

            for (auto &&textureState : command->getTextureState()) {
                this->barrier->recordTextureBarrier(this->commandBuffer.get(), textureState.target, textureState.state,
                    textureState.stage, textureState.access);
            }
            
            command->execute(this->commandBuffer.get());
        }

        for (auto &&command : this->commandList) {
            delete command;
        }

        return this->commandBuffer;
    }

    std::vector<BufferCommandState> BeginRenderPassCommand::getBufferState() {
        return {};
    }

    std::vector<TextureCommandState> BeginRenderPassCommand::getTextureState() {
        std::vector<TextureCommandState> textureCommandStates;

        for (auto &&colorTextureView : this->colorTextureViews) {
            textureCommandStates.emplace_back(TextureCommandState{
                .target = colorTextureView,
                .state = TextureState::eColorAttachment,
                .stage = PipelineStage::eAttachmentOutput,
                .access = ResourceAccess::eWriteOnly
            });
        }

        if (this->depthStencilTextureView != nullptr) {
            textureCommandStates.emplace_back(TextureCommandState{
                .target = this->depthStencilTextureView,
                .state = TextureState::eDepthStencilAttachment,
                .stage = PipelineStage::eLateFragmentTest,
                .access = ResourceAccess::eWriteOnly
            });
        }

        return textureCommandStates;
    }
}
