#pragma once

#include <module_render/RenderPass.hpp>
#include <module_render/util/ShaderFactory.hpp>

namespace lib::render {

class BasicTriangle final : public RenderPass {
public:
    BasicTriangle(const nvrhi::DeviceHandle& device, const std::shared_ptr<ShaderFactory>& shader_factory);

    void update_frame(const FrameInterval& interval) override;
    void render(nvrhi::IFramebuffer* frame_buffer) override;
    void back_buffer_resizing() override;
    void back_buffer_resized(const point2Di& size) override;

private:
    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;
    nvrhi::GraphicsPipelineHandle m_Pipeline;
    nvrhi::CommandListHandle m_CommandList;

};

}