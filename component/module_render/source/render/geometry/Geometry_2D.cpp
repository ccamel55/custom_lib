#include <module_render/render/geometry/Geometry_2D.hpp>

using namespace lib::render;

Geometry_2D::Geometry_2D(
    const nvrhi::DeviceHandle& device,
    const std::filesystem::path& shader_folder,
    const std::filesystem::path& texture_folder,
    const std::unique_ptr<ShaderFactory>& shader_factory,
    const std::unique_ptr<TextureFactory>& texture_factory
)
    : Geometry_Common(device)
    , _vertices(detail::MAX_VERTICES)
    , _indices(detail::MAX_INDICES) {

    // Load shaders
    const auto vertex_shader    = shader_factory->create_shader(shader_folder / "geometry_2d_vs.spv", nvrhi::ShaderType::Vertex);
    if (!vertex_shader.has_value()) {
        throw std::runtime_error("Could not load vertex shaders from disk: " + vertex_shader.error());
    }

    _vertex_shader = vertex_shader.value();

    const auto pixel_shader     = shader_factory->create_shader(shader_folder / "geometry_2d_ps.spv", nvrhi::ShaderType::Pixel);
    if (!pixel_shader.has_value()) {
        throw std::runtime_error("Could not load pixel shaders from disk: " + pixel_shader.error());
    }

    _pixel_shader = pixel_shader.value();

    // Load texture
    const auto texture = texture_factory->create_texture(texture_folder / "cat.jpg", TextureColor::RGBA);
    if (!texture.has_value()) {
        throw std::runtime_error("Could not load texture from disk: " + texture.error());
    }

    _texture = texture.value();

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

    _vertex_buffer = buffer_object_t::create(_device, desc);

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

    _index_buffer = buffer_object_t::create(_device, desc);

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

    _constant_buffer = buffer_object_t::create(_device, desc);

    // Create vertex layout
    nvrhi::VertexAttributeDesc vertex_attributes[3];
    {
        {
            auto& attribute = vertex_attributes[0];

            attribute.name          = "POSITION";
            attribute.format        = nvrhi::Format::RGB32_FLOAT;
            attribute.arraySize     = 1;
            attribute.bufferIndex   = 0;
            attribute.offset        = offsetof(detail::vertex_t, position);
            attribute.elementStride = sizeof(detail::vertex_t);
        }

        {
            auto& attribute = vertex_attributes[1];

            attribute.name          = "UV";
            attribute.format        = nvrhi::Format::RG32_FLOAT;
            attribute.arraySize     = 1;
            attribute.bufferIndex   = 0;
            attribute.offset        = offsetof(detail::vertex_t, uv);
            attribute.elementStride = sizeof(detail::vertex_t);
        }

        {
            auto& attribute = vertex_attributes[2];

            attribute.name          = "COLOR";
            attribute.format        = nvrhi::Format::RGBA8_UNORM;
            attribute.arraySize     = 1;
            attribute.bufferIndex   = 0;
            attribute.offset        = offsetof(detail::vertex_t, color);
            attribute.elementStride = sizeof(detail::vertex_t);
        }
    }
    _input_layout = _device->createInputLayout(vertex_attributes, std::size(vertex_attributes), _vertex_shader);

    // Texture sampler
    nvrhi::SamplerDesc sampler_desc;
    {
        sampler_desc.setAllFilters(false);
        sampler_desc.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
    }
    _sampler = _device->createSampler(sampler_desc);

    // Create binding set

    nvrhi::BindingSetDesc binding_set_desc;
    {
        binding_set_desc.bindings = {
            nvrhi::BindingSetItem::ConstantBuffer(0, _constant_buffer.buffer(), nvrhi::BufferRange(0, sizeof(detail::constant_buffer_t))),
            nvrhi::BindingSetItem::Texture_SRV(0, _texture),
            nvrhi::BindingSetItem::Sampler(0, _sampler)
        };
    }

    // Create the binding layout (if it's empty -- so, on the first iteration) and the binding set.
    if (!nvrhi::utils::CreateBindingSetAndLayout(_device, nvrhi::ShaderType::All, 0, binding_set_desc, _binding_layout, _binding_set)) {
        throw std::runtime_error("Could not create binding set or layout");
    }

    _command_list = _device->createCommandList();
}

void Geometry_2D::draw_geometry(nvrhi::IFramebuffer* frame_buffer) {

    const nvrhi::FramebufferInfoEx& frame_buffer_info = frame_buffer->getFramebufferInfo();

    buffer_object_t vertex_buffer_1 = buffer_object_t::create(_vertex_buffer, sizeof(detail::vertex_t), 12).value();
    buffer_object_t index_buffer_1  = buffer_object_t::create(_index_buffer, sizeof(detail::index_t), 12).value();

    if (!_pipeline) {
        nvrhi::GraphicsPipelineDesc pipeline_desc;
        {
            pipeline_desc.VS                = _vertex_shader;
            pipeline_desc.PS                = _pixel_shader;
            pipeline_desc.inputLayout       = _input_layout;
            pipeline_desc.bindingLayouts    = { _binding_layout };
            pipeline_desc.primType          = nvrhi::PrimitiveType::TriangleList;

            pipeline_desc.renderState.blendState.targets[0].blendEnable     = true;
            pipeline_desc.renderState.blendState.targets[0].srcBlend        = nvrhi::BlendFactor::SrcAlpha;
            pipeline_desc.renderState.blendState.targets[0].destBlend       = nvrhi::BlendFactor::InvSrcAlpha;
            pipeline_desc.renderState.blendState.targets[0].srcBlendAlpha   = nvrhi::BlendFactor::InvSrcAlpha;
            pipeline_desc.renderState.blendState.targets[0].destBlendAlpha  = nvrhi::BlendFactor::Zero;

            pipeline_desc.renderState.depthStencilState.depthTestEnable     = false;
            pipeline_desc.renderState.depthStencilState.depthWriteEnable    = true;
            pipeline_desc.renderState.depthStencilState.stencilEnable       = false;
            pipeline_desc.renderState.depthStencilState.depthFunc           = nvrhi::ComparisonFunc::Always;

            pipeline_desc.renderState.rasterState.scissorEnable         = true;
            pipeline_desc.renderState.rasterState.frontCounterClockwise = false;
        }
        _pipeline = _device->createGraphicsPipeline(pipeline_desc, frame_buffer);
    }

    _command_list->open();
    {
        if (_num_indices != 0 && _num_vertices != 0) {

            vertex_buffer_1.write(_command_list, _vertices.data(), _num_vertices * sizeof(detail::vertex_t));
            index_buffer_1.write(_command_list, _indices.data(), _num_indices * sizeof(detail::index_t));

            detail::constant_buffer_t constants;
            {
                constants.view_matrix = glm::ortho(
                   0.0, static_cast<double>(frame_buffer_info.width),
                   static_cast<double>(frame_buffer_info.height), 0.0
               );
            }
            _constant_buffer.write(_command_list, &constants, sizeof(detail::constant_buffer_t));

            nvrhi::GraphicsState state;
            {
                state.bindings      = { _binding_set };
                state.indexBuffer   = { index_buffer_1.buffer(), nvrhi::Format::R32_UINT, 0 };
                state.vertexBuffers = { { vertex_buffer_1.buffer(), 0, 0 } };

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
                draw_arguments.startVertexLocation = 1;
                draw_arguments.startIndexLocation = 1;
                draw_arguments.vertexCount = _num_indices;
            }
            _command_list->drawIndexed(draw_arguments);

            _num_vertices   = 0;
            _num_indices    = 0;
        }
    }
    _command_list->close();
    _device->executeCommandList(_command_list);
}

void Geometry_2D::back_buffer_resizing() {
    // setting this to null will re-create pipelines on next render
    _pipeline = nullptr;
}

