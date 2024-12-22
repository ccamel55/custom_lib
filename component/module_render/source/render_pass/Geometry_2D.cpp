#include <module_render/render_pass/Geometry_2D.hpp>

using namespace lib::render;

namespace {

}

Geometry_2D::Geometry_2D(Device_Common* backend)
    : RenderPass(backend) {

}

void Geometry_2D::init(
    const std::filesystem::path& shader_folder,
    const std::unique_ptr<ShaderFactory>& shader_factory,
    const std::unique_ptr<TextureFactory>& texture_factory
) {
    // Load shaders
    const auto vertex_shader    = shader_factory->create_shader(shader_folder / "geometry_2d_vs.spv", nvrhi::ShaderType::Vertex);
    const auto pixel_shader     = shader_factory->create_shader(shader_folder / "geometry_2d_ps.spv", nvrhi::ShaderType::Pixel);

    if (!vertex_shader.has_value()) {
        throw std::runtime_error("Could not load vertex shaders from disk: " + vertex_shader.error());
    }

    if (!pixel_shader.has_value()) {
        throw std::runtime_error("Could not load pixel shaders from disk: " + pixel_shader.error());
    }

    _vertex_shader  = vertex_shader.value();
    _pixel_shader   = pixel_shader.value();

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

    _vertex_buffer = device()->createBuffer(desc);

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

    _index_buffer = device()->createBuffer(desc);

    // Constant Buffer
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

    _constant_buffer = device()->createBuffer(desc);

    // Create vertex layout
    nvrhi::VertexAttributeDesc vertex_attributes[3];
    {
        {
            auto& attribute = vertex_attributes[0];

            attribute.name          = "POSITION";
            attribute.format        = nvrhi::Format::RGB32_FLOAT;
            attribute.offset        = 0;
            attribute.bufferIndex   = 0;
            attribute.elementStride = sizeof(detail::vertex_t);
        }

        {
            auto& attribute = vertex_attributes[1];

            attribute.name          = "UV";
            attribute.format        = nvrhi::Format::RG32_FLOAT;
            attribute.offset        = 0;
            attribute.bufferIndex   = 1;
            attribute.elementStride = sizeof(detail::vertex_t);
        }

        {
            auto& attribute = vertex_attributes[2];

            attribute.name          = "COLOR";
            attribute.format        = nvrhi::Format::RGBA32_FLOAT;
            attribute.offset        = 0;
            attribute.bufferIndex   = 2;
            attribute.elementStride = sizeof(detail::vertex_t);
        }
    }
    _vertex_layout = device()->createInputLayout(vertex_attributes, std::size(vertex_attributes), _vertex_shader);

    // Load texture

    _command_list = device()->createCommandList();
    _command_list->open();
    {
        // Todo: load texture
    }
    _command_list->close();
    device()->executeCommandList(_command_list);

    // Create binding set

    nvrhi::BindingSetDesc binding_set_desc;
    {
        // Todo: add texture
        // Todo: add sampler

        binding_set_desc.bindings = {
            // Note: using viewIndex to construct a buffer range.
            nvrhi::BindingSetItem::ConstantBuffer(0, _constant_buffer, nvrhi::BufferRange(0, sizeof(detail::constant_buffer_t))),
        };
    }

    // Create the binding layout (if it's empty -- so, on the first iteration) and the binding set.
    if (!nvrhi::utils::CreateBindingSetAndLayout(device(), nvrhi::ShaderType::All, 0, binding_set_desc, _binding_layout, _binding_set)) {
        throw std::runtime_error("Could not create binding set or layout");
    }
}

void Geometry_2D::update_frame(const FrameInterval& interval) {

}

void Geometry_2D::render(nvrhi::IFramebuffer* frame_buffer) {

    const nvrhi::FramebufferInfoEx& frame_buffer_info = frame_buffer->getFramebufferInfo();

    if (!_pipeline) {
        nvrhi::GraphicsPipelineDesc pipeline_desc;
        {
            pipeline_desc.VS                = _vertex_shader;
            pipeline_desc.PS                = _pixel_shader;
            pipeline_desc.inputLayout       = _vertex_layout;
            pipeline_desc.bindingLayouts    = { _binding_layout };
            pipeline_desc.primType          = nvrhi::PrimitiveType::TriangleList;
            pipeline_desc.renderState.depthStencilState.depthTestEnable = false;
        }
        _pipeline = device()->createGraphicsPipeline(pipeline_desc, frame_buffer);
    }

    _command_list->open();
    {
        nvrhi::utils::ClearColorAttachment(_command_list, frame_buffer, 0, nvrhi::Color(0.f));

        _command_list->writeBuffer(_vertex_buffer, _vertices.data(), std::size(_vertices));
        _command_list->writeBuffer(_index_buffer, _indices.data(), std::size(_indices));
        _command_list->writeBuffer(_constant_buffer, &_constants, sizeof(_constants));

        nvrhi::GraphicsState state;
        {
            state.bindings      = { _binding_set };
            state.indexBuffer   = { _index_buffer, nvrhi::Format::R32_UINT, 0 };

            // Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
            state.vertexBuffers = {
                { _vertex_buffer, 0, offsetof(detail::vertex_t, position) },
                { _vertex_buffer, 1, offsetof(detail::vertex_t, uv) },
                { _vertex_buffer, 2, offsetof(detail::vertex_t, color) },
            };

            state.pipeline      = _pipeline;
            state.framebuffer   = frame_buffer;

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
            draw_arguments.vertexCount = _indices.size();
        }
        _command_list->drawIndexed(draw_arguments);
    }
    _command_list->close();
    device()->executeCommandList(_command_list);
}

void Geometry_2D::back_buffer_resizing() {
    _pipeline = nullptr;
}

void Geometry_2D::back_buffer_resized(const point2Di& size) {

    // Temporary draw shit
    // todo: implement properly

    _constants.view_projection = glm::ortho(
        -2.0, 2.0,
        -2.0, 2.0
    );

    _vertices = {
        detail::vertex_t{ { -0.5, -0.5 },  { 0.0, 0.0 }, { 1.0, 0.0, 0.0 } },
        detail::vertex_t{ {  0.0,  0.5 },  { 0.0, 0.0 }, { 0.0, 1.0, 0.0 } },
        detail::vertex_t{ {  0.5, -0.5 },  { 0.0, 0.0 }, { 0.0, 0.0, 1.0 } },
    };

    _indices = {
        0, 1, 2,
    };
}



