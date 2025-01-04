#include <module_render/geometry/Geometry_2D.hpp>
#include <module_render/types/hlsl_alias.hpp>

#include <module_render/shaders/types/geometry_cb.h>

using namespace lib::render;

// GLM BASICS:
// http://www.c-jump.com/bcc/common/Talk3/Math/GLM/GLM.html#W01_0040_identity_matrix

namespace {

static_assert(
    sizeof(constant_buffer_t) % nvrhi::c_ConstantBufferOffsetSizeAlignment == 0,
    "sizeof(constant_buffer_t) must be 256 bytes"
);

constexpr uint8_t DEFAULT_TEXTURE_WHITE[] = {
    0xFF, 0xFF, 0xFF, 0xFF
};

}

Geometry_2D::Geometry_2D(
    const nvrhi::DeviceHandle& device,
    const std::shared_ptr<FontFactory>& font_factory,
    const std::shared_ptr<ShaderFactory>& shader_factory,
    const std::shared_ptr<TextureFactory>& texture_factory
)
    : _device(device)
    , _pipeline(_device)
    , _draw(geometry::MAX_VERTICES, geometry::MAX_INDICES)
    , _font_factory(font_factory)
    , _shader_factory(shader_factory)
    , _texture_factory(texture_factory) {

    // Create buffers
    nvrhi::BufferDesc desc;
    {
        desc.byteSize           = _draw.max_vertices() * sizeof(geometry::vertex_t);
        desc.debugName          = "VertexBuffer";
        desc.isVertexBuffer     = true;
        desc.isIndexBuffer      = false;
        desc.isConstantBuffer   = false;
        desc.isVolatile         = false;
        desc.initialState       = nvrhi::ResourceStates::CopyDest;
        desc.keepInitialState   = true;
    }
    _vertex_buffer = buffer_object_t::create(_device, desc);
    {
        desc.byteSize           = _draw.max_indices() * sizeof(geometry::index_t);
        desc.debugName          = "IndexBuffer";
        desc.isVertexBuffer     = false;
        desc.isIndexBuffer      = true;
        desc.isConstantBuffer   = false;
        desc.isVolatile         = false;
        desc.initialState       = nvrhi::ResourceStates::CopyDest;
        desc.keepInitialState   = true;
    }
    _index_buffer = buffer_object_t::create(_device, desc);
    {
        desc.byteSize           = sizeof(constant_buffer_t);
        desc.debugName          = "ConstantBuffer";
        desc.isVertexBuffer     = false;
        desc.isIndexBuffer      = false;
        desc.isConstantBuffer   = true;
        desc.isVolatile         = false;
        desc.initialState       = nvrhi::ResourceStates::ConstantBuffer;
        desc.keepInitialState   = true;
    }
    _constant_buffer = buffer_object_t::create(_device, desc);

    // Create shaders
    {
        auto vertex_shader = shader_factory->create_shader("geometry", nvrhi::ShaderType::Vertex, "main_vs");
        if (!vertex_shader.has_value()) {
            throw std::runtime_error("Could not load vertex shaders from disk: " + vertex_shader.error());
        }
        _vertex_shader = std::move(vertex_shader.value());

        std::vector<ShaderMake::ShaderConstant> constants = {
            { "SDF_ENABLE", "0" }
        };

        auto pixel_shader = shader_factory->create_shader("geometry", nvrhi::ShaderType::Pixel, "main_ps", constants);
        if (!pixel_shader.has_value()) {
            throw std::runtime_error("Could not load pixel shaders from disk: " + pixel_shader.error());
        }
        _pixel_shader = std::move(pixel_shader.value());

        constants.at(0).value = "1";

        auto pixel_shader_sdf = shader_factory->create_shader("geometry", nvrhi::ShaderType::Pixel, "main_ps", constants);
        if (!pixel_shader_sdf.has_value()) {
            throw std::runtime_error("Could not load pixel shaders from disk: " + pixel_shader_sdf.error());
        }
        _pixel_shader_sdf = std::move(pixel_shader_sdf.value());

        constants.at(0).value = "2";

        auto pixel_shader_sdf_outline = shader_factory->create_shader("geometry", nvrhi::ShaderType::Pixel, "main_ps", constants);
        if (!pixel_shader_sdf_outline.has_value()) {
            throw std::runtime_error("Could not load pixel shaders from disk: " + pixel_shader_sdf_outline.error());
        }
        _pixel_shader_sdf_outline = std::move(pixel_shader_sdf_outline.value());

        _vertex_layout = device->createInputLayout(
            geometry::vertex_t::attributes().data(),
            geometry::vertex_t::attributes().size(),
            _vertex_shader
        );
    }

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

    // Load default texture
    {
        auto texture_default = add_texture(DEFAULT_TEXTURE_WHITE, { 1, 1 });
        if (!texture_default.has_value()) {
            throw std::runtime_error("Could not load texture from disk: " + texture_default.error());
        }
        _texture_default = texture_default.value();
    }
}

void Geometry_2D::draw_geometry(const nvrhi::CommandListHandle& command_list, nvrhi::IFramebuffer* frame_buffer) {

    const nvrhi::FramebufferInfoEx& frame_buffer_info = frame_buffer->getFramebufferInfo();

    if (_update_pipeline) {
        _update_pipeline = false;

        _pipeline.back_buffer_resized([&](auto& pipeline) {
            pipeline[static_cast<size_t>(geometry::Pipeline_Id::Geometry_Texture)] = _device->createGraphicsPipeline(
            nvrhi::GraphicsPipelineDesc()
                .setVertexShader(_vertex_shader)
                .setPixelShader(_pixel_shader)
                .setInputLayout(_vertex_layout)
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
                frame_buffer
            );
        });

        _pipeline.back_buffer_resized([&](auto& pipeline) {
            pipeline[static_cast<size_t>(geometry::Pipeline_Id::Geometry_Texture_Sdf)] = _device->createGraphicsPipeline(
            nvrhi::GraphicsPipelineDesc()
                .setVertexShader(_vertex_shader)
                .setPixelShader(_pixel_shader_sdf)
                .setInputLayout(_vertex_layout)
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
                frame_buffer
            );
        });

        _pipeline.back_buffer_resized([&](auto& pipeline) {
            pipeline[static_cast<size_t>(geometry::Pipeline_Id::Geometry_Texture_Sdf_Outline)] = _device->createGraphicsPipeline(
            nvrhi::GraphicsPipelineDesc()
                .setVertexShader(_vertex_shader)
                .setPixelShader(_pixel_shader_sdf_outline)
                .setInputLayout(_vertex_layout)
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
                frame_buffer
            );
        });
    }

    // Write constant buffer
    if (_update_constant_buffer) {
        _update_constant_buffer = false;

        constant_buffer_t constants = {};
        {
            // 2D doesn't need any model or view matrix changes
            constants.model_matrix      = float4x4(1.0);
            constants.view_matrix       = float4x4(1.0);
            constants.projection_matrix = glm::ortho(
               0.0, static_cast<double>(frame_buffer_info.width),
               static_cast<double>(frame_buffer_info.height), 0.0
            );

            // Do multiplication once here, can use in vertex shader later
            constants.mvp_matrix = constants.projection_matrix
                * constants.view_matrix
                * constants.model_matrix;
        }
        _constant_buffer.write(command_list, &constants, sizeof(constant_buffer_t));
    }

    // Write draw list
    {
        _vertex_buffer.write(command_list, _draw.backing_vertices.data(), _draw.num_vertices * sizeof(geometry::vertex_t), 0);
        _index_buffer.write(command_list, _draw.backing_indices.data(), _draw.num_indices * sizeof(geometry::index_t), 0);

        for (const auto& batch: _draw.draw_commands) {

            // Get binding set from cache or build it
            nvrhi::BindingSetDesc binding_set_desc;
            {
                binding_set_desc.bindings = {
                    nvrhi::BindingSetItem::ConstantBuffer(0, _constant_buffer.buffer(), nvrhi::BufferRange(0, sizeof(constant_buffer_t))),
                    nvrhi::BindingSetItem::Texture_SRV(0, batch.texture->Get()),
                    nvrhi::BindingSetItem::Sampler(0, _sampler)
                };
            }

            nvrhi::BindingSetHandle& binding_set = _binding_set[binding_set_desc_key(binding_set_desc)];
            if (!binding_set) {
                binding_set = _device->createBindingSet(binding_set_desc, _binding_layout);
            }

            nvrhi::GraphicsState state;
            {
                state.bindings      = { binding_set };
                state.indexBuffer   = { _index_buffer.buffer(), nvrhi::Format::R32_UINT, 0 };
                state.vertexBuffers = { { _vertex_buffer.buffer(), 0, 0 } };

                state.pipeline      = _pipeline[batch.pipeline];
                state.framebuffer   = frame_buffer;

                // Construct the viewport so that all viewports form a grid.
                const nvrhi::Viewport viewport = nvrhi::Viewport(
                    0, static_cast<float>(frame_buffer_info.width),
                    0, static_cast<float>(frame_buffer_info.height),
                    0.f, 1.f
                );
                state.viewport.addViewportAndScissorRect(viewport);
            }
            command_list->setGraphicsState(state);

            nvrhi::DrawArguments draw_arguments;
            {
                draw_arguments.startVertexLocation  = 0;
                draw_arguments.startIndexLocation   = batch.offset;
                draw_arguments.vertexCount          = batch.count;
            }
            command_list->drawIndexed(draw_arguments);
        }

        _draw.draw_commands.clear();

        _draw.num_vertices  = 0;
        _draw.num_indices   = 0;
    }
}

void Geometry_2D::back_buffer_resizing() {
    _update_constant_buffer = true;
    _update_pipeline        = true;
}

//
// ------------------------------------------------------------------------------------------------------------------------
//

std::expected<geometry::Texture_Id, std::string> Geometry_2D::add_texture(const std::filesystem::path& path) {
    auto texture = _texture_factory->create_texture(path, TextureColor::RGBA);
    if (!texture.has_value()) {
        return std::unexpected(texture.error());
    }
    return _texture.emplace(_texture.end(), std::move(texture.value()));
}

std::expected<geometry::Texture_Id, std::string> Geometry_2D::add_texture(const uint8_t* data_ptr, const point2Di& size) {
    auto texture = _texture_factory->create_texture(data_ptr, size, TextureColor::RGBA);
    if (!texture.has_value()) {
        return std::unexpected(texture.error());
    }
    return _texture.emplace(_texture.end(), std::move(texture.value()));
}

void Geometry_2D::remove_texture(const geometry::Texture_Id& id) {
    _texture.erase(id);
}

std::expected<geometry::Font_Id, std::string> Geometry_2D::add_font(const std::filesystem::path& path, float height) {
    auto font = _font_factory->load_font(path, height);
    if (!font.has_value()) {
        return std::unexpected(font.error());
    }

    auto texture = add_texture(font->atlas.data(), font->atlas_size);
    if (!texture.has_value()) {
        return std::unexpected(texture.error());
    }

    geometry::font_handle_t font_handle;
    {
        font_handle.texture     = std::move(texture.value());
        font_handle.characters  = font->character;
    }
    return _font.emplace(_font.end(), std::move(font_handle));
}

void Geometry_2D::remove_font(const geometry::Font_Id& id) {
    _font.erase(id);
}

//
// ------------------------------------------------------------------------------------------------------------------------
//

void Geometry_2D::d_texture(const point2Df& pos, const point2Df& size, const geometry::Texture_Id& texture, const uint8_t alpha) {

    _draw.prepare_draw(texture, geometry::Pipeline_Id::Geometry_Texture);

    size_t first_vertex_index;
    std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

    vertices[0] = geometry::vertex_t(pos.x, pos.y, 0.f, 0.f, 0.f, 255, 255, 255, alpha);
    vertices[1] = geometry::vertex_t(pos.x + size.x, pos.y, 0.f, 1.f, 0.f, 255, 255, 255, alpha);
    vertices[2] = geometry::vertex_t(pos.x + size.x, pos.y + size.y, 0.f, 1.f, 1.f, 255, 255, 255, alpha);
    vertices[3] = geometry::vertex_t(pos.x, pos.y + size.y, 0.f, 0.f, 1.f, 255, 255, 255, alpha);

    std::span<geometry::index_t> indices = _draw.emplace_indices(6);

    indices[0] = first_vertex_index + 0;
    indices[1] = first_vertex_index + 1;
    indices[2] = first_vertex_index + 2;

    indices[3] = first_vertex_index + 0;
    indices[4] = first_vertex_index + 2;
    indices[5] = first_vertex_index + 3;
}

void Geometry_2D::d_line(const point2Df& pos_1, const point2Df& pos_2, const color& color, const float thickness) {

    const auto dir = glm::normalize(pos_2 - pos_1) * (thickness * 0.5f);

    _draw.prepare_draw(_texture_default, geometry::Pipeline_Id::Geometry_Texture);

    size_t first_vertex_index;
    std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

    vertices[0] = geometry::vertex_t(pos_1.x + dir.y, pos_1.y - dir.x, 0.f, 0.f, 0.f, color.r, color.g, color.b, color.a);
    vertices[1] = geometry::vertex_t(pos_2.x + dir.y, pos_2.y - dir.x, 0.f, 0.f, 0.f, color.r, color.g, color.b, color.a);
    vertices[2] = geometry::vertex_t(pos_2.x - dir.y, pos_2.y + dir.x, 0.f, 0.f, 0.f, color.r, color.g, color.b, color.a);
    vertices[3] = geometry::vertex_t(pos_1.x - dir.y, pos_1.y + dir.x, 0.f, 0.f, 0.f, color.r, color.g, color.b, color.a);

    std::span<geometry::index_t> indices = _draw.emplace_indices(6);

    indices[0] = first_vertex_index + 0;
    indices[1] = first_vertex_index + 1;
    indices[2] = first_vertex_index + 2;

    indices[3] = first_vertex_index + 0;
    indices[4] = first_vertex_index + 2;
    indices[5] = first_vertex_index + 3;
}

void Geometry_2D::d_text(
    const point2Df& pos,
    const color& color,
    const geometry::Font_Id& id,
    const std::string& text,
    const bitflag flags
) {
    // Fonts will always use SDF rendering, this allows us to scale the size of the font in the future
    // TODO: implement font scaling
    _draw.prepare_draw(
        id->texture,
        flags.has(font_flags::Outline)
            ? geometry::Pipeline_Id::Geometry_Texture_Sdf_Outline
            : geometry::Pipeline_Id::Geometry_Texture_Sdf
    );

    // Adjust pos based on flags
    point2Df fixed_pos = pos;

    const auto text_height = [&]() -> float {
        return static_cast<float>(id->characters.at(0).spacing.y);
    };

    const auto text_width = [&]() -> float {
        int w = 0;
        for (const auto& c : text) {
            w += id->characters.at(c - CHAR_START).spacing.x;
        }
        return static_cast<float>(w);
    };

    if (flags.has(font_flags::Align_R)) {
        fixed_pos.x -= text_width();
    }
    else if (flags.has(font_flags::Centre_X)) {
        fixed_pos.x -= text_width() / 2;
    }

    if (flags.has(font_flags::Centre_Y)) {
        fixed_pos.y += text_height() / 2;
    }

    // Add each character by its self
    for (const char c: text) {

        const auto& character = id->characters.at(c - CHAR_START);

        if (c != ' ') {

            const auto aligned_pos  = fixed_pos + point2Df(character.align);
            const auto size         = point2Df(character.size);

            size_t first_vertex_index;
            std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

            vertices[0] = geometry::vertex_t(
                aligned_pos.x, aligned_pos.y, 0.f,
                character.atlas_start.x, character.atlas_start.y,
                color.r, color.g, color.b, color.a
            );

            vertices[1] = geometry::vertex_t(
                aligned_pos.x + size.x, aligned_pos.y, 0.f,
                character.atlas_end.x, character.atlas_start.y,
                color.r, color.g, color.b, color.a
            );

            vertices[2] = geometry::vertex_t(
                aligned_pos.x + size.x, aligned_pos.y + size.y, 0.f,
                character.atlas_end.x, character.atlas_end.y,
                color.r, color.g, color.b, color.a
            );

            vertices[3] = geometry::vertex_t(
                aligned_pos.x, aligned_pos.y + size.y, 0.f,
                character.atlas_start.x, character.atlas_end.y,
                color.r, color.g, color.b, color.a
            );

            std::span<geometry::index_t> indices = _draw.emplace_indices(6);

            indices[0] = first_vertex_index + 0;
            indices[1] = first_vertex_index + 1;
            indices[2] = first_vertex_index + 2;

            indices[3] = first_vertex_index + 0;
            indices[4] = first_vertex_index + 2;
            indices[5] = first_vertex_index + 3;
        }

        fixed_pos.x += static_cast<float>(character.spacing.x);
    }
}

