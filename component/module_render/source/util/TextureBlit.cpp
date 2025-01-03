#include <module_render/util/TextureBlit.hpp>

using namespace lib::render;

namespace {

[[nodiscard]] bool is_supported_blit_dimension(const nvrhi::TextureDimension dimension) {
    return dimension == nvrhi::TextureDimension::Texture2D
        || dimension == nvrhi::TextureDimension::Texture2DArray
        || dimension == nvrhi::TextureDimension::TextureCube
        || dimension == nvrhi::TextureDimension::TextureCubeArray;
}

[[nodiscard]] bool is_texture_array(const nvrhi::TextureDimension dimension) {
    return dimension == nvrhi::TextureDimension::Texture2DArray
        || dimension == nvrhi::TextureDimension::TextureCube
        || dimension == nvrhi::TextureDimension::TextureCubeArray;
}

}

TextureBlit::TextureBlit(
    nvrhi::IDevice* device,
    const std::unique_ptr<ShaderFactory>& shader_factory
)
    : _device(device) {

    auto rect_shader = shader_factory->create_shader("blit_vs", nvrhi::ShaderType::Vertex);
    if (!rect_shader.has_value()) {
        throw std::runtime_error("Could not load rect shader: " + rect_shader.error());
    }

    std::vector<ShaderMake::ShaderConstant> constants = {
        { "TEXTURE_ARRAY", "0" }
    };

    auto blit_shader = shader_factory->create_shader("blit_ps", nvrhi::ShaderType::Pixel);
    if (!blit_shader.has_value()) {
        throw std::runtime_error("Could not load blit shader: " + blit_shader.error());
    }

    constants[0].value = "1";

    auto blit_shader_array = shader_factory->create_shader("blit_ps", nvrhi::ShaderType::Pixel);
    if (!blit_shader_array.has_value()) {
        throw std::runtime_error("Could not load blit array shader: " + blit_shader_array.error());
    }

    _rect_shader        = std::move(rect_shader.value());
    _blit_shader        = std::move(blit_shader.value());
    _blit_shader_array  = std::move(blit_shader_array.value());

    nvrhi::SamplerDesc sampler_desc;

    sampler_desc.setAllFilters(true);
    sampler_desc.setAllAddressModes(nvrhi::SamplerAddressMode::Clamp);

    _linear_sampler = device->createSampler(sampler_desc);

    nvrhi::BindingLayoutDesc layout_desc;

    layout_desc.visibility = nvrhi::ShaderType::All;
    layout_desc.bindings = {
        nvrhi::BindingLayoutItem::PushConstants(0, sizeof(blit_constants_t)),
        nvrhi::BindingLayoutItem::Texture_SRV(0),
        nvrhi::BindingLayoutItem::Sampler(0)
    };

    _blit_layout = device->createBindingLayout(layout_desc);
}

void TextureBlit::back_buffer_resizing() {
    _binding_set.clear();
    _pipeline.clear();
}

void TextureBlit::blit(
    nvrhi::ICommandList* command_list,
    nvrhi::ITexture* source_texture,
    nvrhi::IFramebuffer* dest_frame_buffer,
    const uint32_t source_array_slice,
    const uint32_t source_mip_level,
    const nvrhi::BlendState::RenderTarget& blend_state,
    const nvrhi::Color& blend_color
) {
    const nvrhi::TextureDesc& source_texture_desc = source_texture->getDesc();

    if (!is_supported_blit_dimension(source_texture_desc.dimension)) [[unlikely]] {
        throw std::runtime_error("Attempted to blit a texture with unsupported dimensions");
    }

    const nvrhi::FramebufferInfoEx& frame_buffer_info = dest_frame_buffer->getFramebufferInfo();
    const bool is_array = is_texture_array(source_texture_desc.dimension);

    nvrhi::GraphicsPipelineHandle& pipeline = _pipeline[detail::pipeline_desc_key(
        frame_buffer_info.width,
        frame_buffer_info.height,
        frame_buffer_info,
        blend_state,
        is_array
    )];

    if (!pipeline) {
        nvrhi::GraphicsPipelineDesc desc;
        {
            desc.VS                 = _rect_shader;
            desc.PS                 = is_array ? _blit_shader_array : _blit_shader;
            desc.primType           = nvrhi::PrimitiveType::TriangleStrip;
            desc.bindingLayouts     = { _blit_layout };

            desc.renderState.rasterState.setCullNone();

            desc.renderState.depthStencilState.depthTestEnable  = false;
            desc.renderState.depthStencilState.stencilEnable    = false;

            desc.renderState.blendState.targets[0] = blend_state;
        }
        pipeline = _device->createGraphicsPipeline(desc, dest_frame_buffer);
    }

    nvrhi::BindingSetDesc binding_set_desc;
    {
        nvrhi::TextureDimension source_dimension = source_texture_desc.dimension;

        if (source_dimension == nvrhi::TextureDimension::TextureCube ||
            source_dimension == nvrhi::TextureDimension::TextureCubeArray
        ) {
            source_dimension = nvrhi::TextureDimension::Texture2DArray;
        }

        const nvrhi::TextureSubresourceSet source_sub_resource(source_mip_level, 1, source_array_slice, 1);

        binding_set_desc.bindings = {
            nvrhi::BindingSetItem::PushConstants(0, sizeof(blit_constants_t)),
            nvrhi::BindingSetItem::Texture_SRV(0, source_texture, nvrhi::Format::UNKNOWN, source_sub_resource, source_dimension),
            nvrhi::BindingSetItem::Sampler(0, _linear_sampler)
        };
    }

    nvrhi::BindingSetHandle& binding_set = _binding_set[detail::binding_set_desc_key(binding_set_desc)];
    if (!binding_set) {
        binding_set = _device->createBindingSet(binding_set_desc, _blit_layout);
    }

    const nvrhi::Viewport viewport = nvrhi::Viewport(
        static_cast<float>(frame_buffer_info.width),
        static_cast<float>(frame_buffer_info.height)
    );

    nvrhi::GraphicsState state;
    {
        state.pipeline              = pipeline;
        state.framebuffer           = dest_frame_buffer;
        state.bindings              = { binding_set };
        state.blendConstantColor    = blend_color;

        state.viewport.addViewportAndScissorRect(viewport);
    }
    command_list->setGraphicsState(state);

    blit_constants_t blit_constants;
    {
        blit_constants.s_origin= float2(0.0, 0.0);
        blit_constants.s_size   = float2(1.0, 1.0);

        blit_constants.d_origin = float2(0.0, 0.0);
        blit_constants.d_size   = float2(1.0, 1.0);
    }
    command_list->setPushConstants(&blit_constants, sizeof(blit_constants_t));

    nvrhi::DrawArguments args;
    {
        args.instanceCount = 1;
        args.vertexCount = 4;
    }
    command_list->draw(args);
}