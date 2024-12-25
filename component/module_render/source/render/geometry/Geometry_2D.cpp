#include <module_render/render/geometry/Geometry_2D.hpp>

using namespace lib::render;

// GLM BASICS:
// http://www.c-jump.com/bcc/common/Talk3/Math/GLM/GLM.html#W01_0040_identity_matrix

namespace {

enum class Blit_ID: uint32_t {
    Color_To_FrameBuffer
};

}

Geometry_2D::Geometry_2D(
    const nvrhi::DeviceHandle& device,
    const std::unique_ptr<ShaderFactory>& shader_factory,
    const std::unique_ptr<TextureFactory>& texture_factory
)
    : Geometry_Common(device)
    , _blit(device, shader_factory, "rect_vs.spv", "blit_ps.spv", "blit_array_ps.spv") {

    // Create buffers
    nvrhi::BufferDesc desc;

    // Vertex Buffer
    {
        desc.byteSize           = sizeof(detail::vertex_t) * detail::MAX_VERTICES;
        desc.debugName          = "VertexBuffer";
        desc.isVertexBuffer     = true;
        desc.isIndexBuffer      = false;
        desc.isConstantBuffer   = false;
        desc.isVolatile         = false;
        desc.initialState       = nvrhi::ResourceStates::CopyDest;
        desc.keepInitialState   = true;
    }

    const auto vertex_buffer = buffer_object_t::create(_device, desc);

    // Index Buffer
    {
        desc.byteSize           = sizeof(detail::index_t) * detail::MAX_INDICES;
        desc.debugName          = "IndexBuffer";
        desc.isVertexBuffer     = false;
        desc.isIndexBuffer      = true;
        desc.isConstantBuffer   = false;
        desc.isVolatile         = false;
        desc.initialState       = nvrhi::ResourceStates::CopyDest;
        desc.keepInitialState   = true;
    }

    const auto index_buffer = buffer_object_t::create(_device, desc);

    _draw = draw_list_t<detail::vertex_t, detail::index_t>::create(vertex_buffer, index_buffer);

    // Create shader
    {
        auto vertex_shader = shader_factory->create_shader("geometry_2d_vs.spv", nvrhi::ShaderType::Vertex);
        if (!vertex_shader.has_value()) {
            throw std::runtime_error("Could not load vertex shaders from disk: " + vertex_shader.error());
        }

        auto pixel_shader = shader_factory->create_shader("geometry_2d_ps.spv", nvrhi::ShaderType::Pixel);
        if (!pixel_shader.has_value()) {
            throw std::runtime_error("Could not load pixel shaders from disk: " + pixel_shader.error());
        }

        _shader = shader_program_t::create(
            _device,
            std::move(vertex_shader.value()),
            std::move(pixel_shader.value()),
            detail::vertex_t::attributes()
        );
    }

    // Create binding set
    {
        desc.byteSize           = sizeof(detail::constant_buffer_t);
        desc.debugName          = "ConstantBuffer";
        desc.isVertexBuffer     = false;
        desc.isIndexBuffer      = false;
        desc.isConstantBuffer   = true;
        desc.isVolatile         = false;
        desc.initialState       = nvrhi::ResourceStates::ConstantBuffer;
        desc.keepInitialState   = true;
    }

    _constant_buffer = buffer_object_t::create(_device, desc);

    // Load texture
    auto texture = texture_factory->create_texture("cat.jpg", TextureColor::RGBA);
    if (!texture.has_value()) {
        throw std::runtime_error("Could not load texture from disk: " + texture.error());
    }

    _texture = std::move(texture.value());

    // Texture sampler
    nvrhi::SamplerDesc sampler_desc;
    {
        sampler_desc.setAllFilters(false);
        sampler_desc.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
    }
    _sampler = _device->createSampler(sampler_desc);

    nvrhi::BindingSetDesc binding_set_desc;
    {
        binding_set_desc.bindings = {
            nvrhi::BindingSetItem::ConstantBuffer(0, _constant_buffer.buffer(), nvrhi::BufferRange(0, sizeof(detail::constant_buffer_t))),
            nvrhi::BindingSetItem::Texture_SRV(0, _texture),
            nvrhi::BindingSetItem::Sampler(0, _sampler)
        };
    }

    auto bindings = bindings_t::create(_device, binding_set_desc);
    if (!bindings.has_value()) {
        throw std::runtime_error("Could not create binding set or layout");
    }

    _binding = std::move(bindings.value());
    _command_list = _device->createCommandList();
}

void Geometry_2D::draw_geometry(nvrhi::IFramebuffer* frame_buffer) {

    const nvrhi::FramebufferInfoEx& frame_buffer_info = _color_frame_buffer->getFramebufferInfo();

    if (!_pipeline) {
        nvrhi::GraphicsPipelineDesc pipeline_desc;
        {
            pipeline_desc.VS                = _shader.vertex_shader;
            pipeline_desc.PS                = _shader.pixel_shader;
            pipeline_desc.inputLayout       = _shader.vertex_layout;
            pipeline_desc.bindingLayouts    = { _binding.layout };
            pipeline_desc.primType          = nvrhi::PrimitiveType::TriangleList;

            pipeline_desc.renderState.blendState.targets[0].blendEnable     = true;

            pipeline_desc.renderState.blendState.targets[0].srcBlend        = nvrhi::BlendFactor::SrcAlpha;
            pipeline_desc.renderState.blendState.targets[0].srcBlendAlpha   = nvrhi::BlendFactor::One;

            pipeline_desc.renderState.blendState.targets[0].destBlend       = nvrhi::BlendFactor::OneMinusSrcAlpha;
            pipeline_desc.renderState.blendState.targets[0].destBlendAlpha  = nvrhi::BlendFactor::Zero;

            pipeline_desc.renderState.depthStencilState.depthTestEnable     = false;
            pipeline_desc.renderState.depthStencilState.depthWriteEnable    = false;
            pipeline_desc.renderState.depthStencilState.stencilEnable       = false;
            pipeline_desc.renderState.depthStencilState.depthFunc           = nvrhi::ComparisonFunc::Less;

            pipeline_desc.renderState.rasterState.scissorEnable         = true;
            pipeline_desc.renderState.rasterState.frontCounterClockwise = false;
        }
        _pipeline = _device->createGraphicsPipeline(pipeline_desc, _color_frame_buffer);
    }

    _command_list->open();
    // Draw to our color target
    {
        detail::constant_buffer_t constants;
        {
            // 2D doesn't need any model or view matrix changes
            constants.model_matrix      = matrix4x4f(1.0);
            constants.view_matrix       = matrix4x4f(1.0);
            constants.projection_matrix = glm::ortho(
               0.0, static_cast<double>(frame_buffer_info.width),
               static_cast<double>(frame_buffer_info.height), 0.0
            );

            // Do multiplication once here, can use in vertex shader later
            constants.mvp_matrix = constants.projection_matrix
                * constants.view_matrix
                * constants.model_matrix;
        }
        _constant_buffer.write(_command_list, &constants, sizeof(detail::constant_buffer_t));

        const auto draw_indices = _draw.update_buffers(_command_list);

        nvrhi::GraphicsState state;
        {
            state.bindings      = { _binding.set };
            state.indexBuffer   = { _draw.index_buffer.buffer(), _draw.index_format(), 0 };
            state.vertexBuffers = { { _draw.vertex_buffer.buffer(), 0, 0 } };

            state.pipeline      = _pipeline;
            state.framebuffer   = _color_frame_buffer;

            // Construct the viewport so that all viewports form a grid.
            const nvrhi::Viewport viewport = nvrhi::Viewport(
                0, static_cast<float>(frame_buffer_info.width),
                0, static_cast<float>(frame_buffer_info.height),
                0.f, 1.f
            );
            state.viewport.addViewportAndScissorRect(viewport);
        }
        _command_list->setGraphicsState(state);

        nvrhi::DrawArguments draw_arguments;
        {
            draw_arguments.startVertexLocation  = 0;
            draw_arguments.startIndexLocation   = 0;
            draw_arguments.vertexCount          = draw_indices;
        }
        _command_list->drawIndexed(draw_arguments);
    }
    // Blit current draw state to frame buffer
    {
        // BS DRAW HERE
        _blit.blit(static_cast<uint32_t>(Blit_ID::Color_To_FrameBuffer), _command_list, _color_buffer, frame_buffer);
    }
    _command_list->close();
    _device->executeCommandList(_command_list);
}

void Geometry_2D::back_buffer_resizing() {
    // setting this to null will re-create pipelines on next render
    _pipeline = nullptr;
    _blit.back_buffer_resizing();
}

void Geometry_2D::back_buffer_resized(const point2Di& size) {

    // Recreate color target and frame buffer
    nvrhi::TextureDesc texture_desc;
    {
        texture_desc.debugName  = "ColorTarget";
        texture_desc.format     = SWAP_CHAIN_FORMAT;
        texture_desc.width      = std::max(size.x, 1);
        texture_desc.height     = std::max(size.y, 1);

        texture_desc.isRenderTarget     = true;
        texture_desc.keepInitialState   = true;
        texture_desc.initialState       = nvrhi::ResourceStates::RenderTarget;
    }
    _color_buffer = _device->createTexture(texture_desc);

    nvrhi::FramebufferDesc frame_buffer_desc;
    frame_buffer_desc.addColorAttachment(_color_buffer);

    _color_frame_buffer = _device->createFramebuffer(frame_buffer_desc);
}
