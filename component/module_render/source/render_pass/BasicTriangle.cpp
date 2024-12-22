#include <module_render/render_pass/BasicTriangle.hpp>

#include <module_system/filesystem.hpp>

using namespace lib::render;

BasicTriangle::BasicTriangle(Device_Common* backend)
    : RenderPass(backend) {

    // Todo: populate with actual render things, not just bogus test stuff
    const auto shader_directory = system::get_executable_path().value().parent_path() / "shaders";

    const ShaderFactory shaders_factory(backend->device());

    m_VertexShader  = shaders_factory.create_shader(shader_directory / "main_vs.spv", nvrhi::ShaderType::Vertex).value();
    m_PixelShader   = shaders_factory.create_shader(shader_directory / "main_ps.spv", nvrhi::ShaderType::Pixel).value();

    m_CommandList = backend->device()->createCommandList();
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
        m_Pipeline = _backend->device()->createGraphicsPipeline(psoDesc, frame_buffer);
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
    _backend->device()->executeCommandList(m_CommandList);
}

void BasicTriangle::back_buffer_resizing() {
    m_Pipeline = nullptr;
}

void BasicTriangle::back_buffer_resized(const point2Di& size) {

}



