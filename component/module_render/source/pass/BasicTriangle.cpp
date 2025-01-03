#include <module_render/pass/BasicTriangle.hpp>
#include <module_system/filesystem.hpp>

using namespace lib::render;

BasicTriangle::BasicTriangle(const nvrhi::DeviceHandle& device, const std::unique_ptr<ShaderFactory>& shader_factory)
    : RenderPass(device) {

    m_VertexShader  = shader_factory->create_shader("main", nvrhi::ShaderType::Vertex, "main_vs").value();
    m_PixelShader   = shader_factory->create_shader("main", nvrhi::ShaderType::Pixel, "main_ps").value();

    m_CommandList = _device->createCommandList();
}

void BasicTriangle::update_frame(const FrameInterval& interval) {

}

void BasicTriangle::render(nvrhi::IFramebuffer* frame_buffer) {
    if (!m_Pipeline) {
        nvrhi::GraphicsPipelineDesc psoDesc;
        {
            psoDesc.VS = m_VertexShader;
            psoDesc.PS = m_PixelShader;
            psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
            psoDesc.renderState.depthStencilState.depthTestEnable = false;
        }
        m_Pipeline = _device->createGraphicsPipeline(psoDesc, frame_buffer);
    }

    m_CommandList->open();

    nvrhi::GraphicsState state;
    state.pipeline = m_Pipeline;
    state.framebuffer = frame_buffer;
    state.viewport.addViewportAndScissorRect(frame_buffer->getFramebufferInfo().getViewport());

    m_CommandList->setGraphicsState(state);

    nvrhi::DrawArguments args;
    args.vertexCount = 3;
    m_CommandList->draw(args);

    m_CommandList->close();
    _device->executeCommandList(m_CommandList);
}

void BasicTriangle::back_buffer_resizing() {
    m_Pipeline = nullptr;
}

void BasicTriangle::back_buffer_resized(const point2Di& size) {

}



