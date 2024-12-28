#include <module_render/render/geometry/Geometry_2D.hpp>

using namespace lib::render;

// GLM BASICS:
// http://www.c-jump.com/bcc/common/Talk3/Math/GLM/GLM.html#W01_0040_identity_matrix

Geometry_2D::Geometry_2D(
    const nvrhi::DeviceHandle& device,
    const std::unique_ptr<ShaderFactory>& shader_factory,
    const std::unique_ptr<TextureFactory>& texture_factory
)
    : Geometry_Common(device)
    , _blit(device, shader_factory, "rect_vs.spv", "blit_ps.spv", "blit_array_ps.spv")
    , _image(device)
    , _frame_buffer(device)
    , _pipeline(_device) {

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

    nvrhi::BindingLayoutDesc binding_layout_desc;
    {
        binding_layout_desc.visibility = nvrhi::ShaderType::All;
        binding_layout_desc.bindings = {
            nvrhi::BindingLayoutItem::ConstantBuffer(0),
            nvrhi::BindingLayoutItem::Texture_SRV(0),
            nvrhi::BindingLayoutItem::Sampler(0)
        };
    }
    _binding_layout = _device->createBindingLayout(binding_layout_desc);

    _command_list       = _device->createCommandList();
    _command_list_blit  = _device->createCommandList();
}

void Geometry_2D::draw_geometry(nvrhi::IFramebuffer* frame_buffer) {

    // All this shit can be done somewhere else since it writes to our color buffer texture, not the frame buffer
    // Note: yes this even means multithreaded rendering using different command lists
    const bool command_list_update = update_vertex || update_constant;

    if (command_list_update) {

        // Get binding set from cache or build it
        nvrhi::BindingSetDesc binding_set_desc;
        {
            binding_set_desc.bindings = {
                nvrhi::BindingSetItem::ConstantBuffer(0, _constant_buffer.buffer(), nvrhi::BufferRange(0, sizeof(detail::constant_buffer_t))),
                nvrhi::BindingSetItem::Texture_SRV(0, _texture),
                nvrhi::BindingSetItem::Sampler(0, _sampler)
            };
        }

        nvrhi::BindingSetHandle& binding_set = _binding_set[detail::binding_set_desc_key(binding_set_desc)];
        if (!binding_set) {
            binding_set = _device->createBindingSet(binding_set_desc, _binding_layout);
        }

        const nvrhi::FramebufferInfoEx& frame_buffer_info = _frame_buffer[FrameBuffer_Id::Geometry]->getFramebufferInfo();

        _command_list->open();
        {
            if (update_vertex) {
                _vertex_count = _draw.update_buffers(_command_list);
            }

            if (update_constant) {
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
            }

            nvrhi::GraphicsState state;
            {
                state.bindings      = { binding_set };
                state.indexBuffer   = { _draw.index_buffer.buffer(), _draw.index_format(), 0 };
                state.vertexBuffers = { { _draw.vertex_buffer.buffer(), 0, 0 } };

                state.pipeline      = _pipeline[Pipeline_Id::Geometry_Texture];
                state.framebuffer   = _frame_buffer[FrameBuffer_Id::Geometry];

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
                draw_arguments.vertexCount          = _vertex_count;
            }
            _command_list->drawIndexed(draw_arguments);
        }
        _command_list->close();

        update_vertex   = false;
        update_constant = false;
    }

    // This must be called here, it will blit the color target to our currently presented frame buffer
    _command_list_blit->open();
    {
        _blit.blit(_command_list_blit, _image[Image_Id::Geometry_ColorTarget], frame_buffer);
    }
    _command_list_blit->close();

    if (command_list_update) {
        const std::array<nvrhi::ICommandList*, 2> command_lists = {
            _command_list,
            _command_list_blit
        };
        _device->executeCommandLists(command_lists.data(), command_lists.size());
    }
    else {
        _device->executeCommandList(_command_list_blit);
    }
}

void Geometry_2D::back_buffer_resizing() {
    // setting this to null will re-create pipelines on next render
    _blit.back_buffer_resizing();
}

void Geometry_2D::back_buffer_resized(const point2Di& size) {

    // Force constant buffer to be re-calculated using the current frame buffer size
    update_constant = true;

    _image.back_buffer_resized([&](auto& image) {
        image[static_cast<size_t>(Image_Id::Geometry_ColorTarget)] = _device->createTexture(
            nvrhi::TextureDesc()
                .setDebugName("ColorTarget")
                .setFormat(SWAP_CHAIN_FORMAT)
                .setWidth(std::max(size.x, 1))
                .setHeight(std::max(size.y, 1))
                .setIsRenderTarget(true)
                .setKeepInitialState(true)
                .setInitialState(nvrhi::ResourceStates::RenderTarget)
        );

        image[static_cast<size_t>(Image_Id::Geometry_DepthTarget)] = _device->createTexture(
            nvrhi::TextureDesc()
                .setDebugName("DepthTarget")
                .setFormat(nvrhi::Format::D32)
                .setWidth(std::max(size.x, 1))
                .setHeight(std::max(size.y, 1))
                .setIsRenderTarget(true)
                .setKeepInitialState(true)
                .setInitialState(nvrhi::ResourceStates::DepthWrite)
        );
    });

    _frame_buffer.back_buffer_resized([&](auto& frame_buffer) {
        frame_buffer[static_cast<size_t>(FrameBuffer_Id::Geometry)] = _device->createFramebuffer(
            nvrhi::FramebufferDesc()
                .addColorAttachment(_image[Image_Id::Geometry_ColorTarget])
                .setDepthAttachment(_image[Image_Id::Geometry_DepthTarget])
        );
    });

    _pipeline.back_buffer_resized([&](auto& pipeline) {
        pipeline[static_cast<size_t>(Pipeline_Id::Geometry_Texture)] = _device->createGraphicsPipeline(
            nvrhi::GraphicsPipelineDesc()
                .setVertexShader(_shader.vertex_shader)
                .setPixelShader(_shader.pixel_shader)
                .setInputLayout(_shader.vertex_layout)
                .addBindingLayout(_binding_layout)
                .setPrimType(nvrhi::PrimitiveType::TriangleList)
                .setRenderState(
                    nvrhi::RenderState()
                        .setBlendState(
                            nvrhi::BlendState()
                                .setRenderTarget(
                                    0,
                                    nvrhi::BlendState::RenderTarget()
                                        .setBlendEnable(true)
                                        .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
                                        .setSrcBlendAlpha(nvrhi::BlendFactor::One)
                                        .setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha)
                                        .setDestBlendAlpha(nvrhi::BlendFactor::OneMinusSrcAlpha)
                                )
                        )
                        .setDepthStencilState(
                            nvrhi::DepthStencilState()
                                .disableDepthWrite()
                                .setDepthTestEnable(false)
                                .setStencilEnable(false)
                                .setDepthFunc(nvrhi::ComparisonFunc::Less)
                        )
                        .setRasterState(
                            nvrhi::RasterState()
                                .setScissorEnable(true)
                                .setFrontCounterClockwise(false)
                        )
                ),
                _frame_buffer[FrameBuffer_Id::Geometry]
        );
    });
}
