#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_render/util/ShaderFactory.hpp>

namespace lib::render {
namespace detail {

constexpr nvrhi::BlendState::RenderTarget DEFAULT_BLEND_STATE = nvrhi::BlendState::RenderTarget()
    .enableBlend()
    .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
    .setSrcBlendAlpha(nvrhi::BlendFactor::One)
    .setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha)
    .setDestBlendAlpha(nvrhi::BlendFactor::OneMinusSrcAlpha)
    .setBlendOp(nvrhi::BlendOp::Add);

[[nodiscard]] inline bool is_supported_blit_dimension(const nvrhi::TextureDimension dimension) {
    return dimension == nvrhi::TextureDimension::Texture2D
        || dimension == nvrhi::TextureDimension::Texture2DArray
        || dimension == nvrhi::TextureDimension::TextureCube
        || dimension == nvrhi::TextureDimension::TextureCubeArray;
}

[[nodiscard]] inline bool is_texture_array(const nvrhi::TextureDimension dimension) {
    return dimension == nvrhi::TextureDimension::Texture2DArray
        || dimension == nvrhi::TextureDimension::TextureCube
        || dimension == nvrhi::TextureDimension::TextureCubeArray;
}

}

template<typename Blit_Id, size_t Num_Ids>
requires std::is_scoped_enum_v<Blit_Id>
class TextureBlit {
    struct blit_instance_t {
        bool operator!() const {
            return !binding_set || !pipeline;
        }

        nvrhi::BindingSetHandle binding_set;
        nvrhi::GraphicsPipelineHandle pipeline;
    };

public:
    TextureBlit(
        nvrhi::IDevice* device,
        const std::unique_ptr<ShaderFactory>& shader_factory,
        const std::filesystem::path& rect_shader_path,
        const std::filesystem::path& blit_shader_path,
        const std::filesystem::path& blit_shader_array_path
    )
        : _device(device) {

        auto rect_shader = shader_factory->create_shader(rect_shader_path, nvrhi::ShaderType::Vertex);
        if (!rect_shader.has_value()) {
            throw std::runtime_error("Could not load rect shader: " + rect_shader.error());
        }

        auto blit_shader = shader_factory->create_shader(blit_shader_path, nvrhi::ShaderType::Pixel);
        if (!blit_shader.has_value()) {
            throw std::runtime_error("Could not load blit shader: " + blit_shader.error());
        }

        auto blit_shader_array = shader_factory->create_shader(blit_shader_array_path, nvrhi::ShaderType::Pixel);
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
            nvrhi::BindingLayoutItem::Texture_SRV(0),
            nvrhi::BindingLayoutItem::Sampler(0)
        };

        _blit_layout = device->createBindingLayout(layout_desc);
    }

    //! Reset pipeline and binding set so we can rebind to new frame buffer texture
    void back_buffer_resizing() {
        for (auto& i : _instances) {
            i.binding_set   = nullptr;
            i.pipeline      = nullptr;
        }
    }

    //! Blit texture from given parameters
    //! \param id blit id, must be convertable to uint32_t
    //! \param command_list command list that will execute the blit
    //! \param source_texture source texture that we want to copy
    //! \param dest_frame_buffer frame buffer we will use to write the source texture
    //! \param source_array_slice array slice/index of texture. Only used for array textures
    //! \param source_mip_level mipmap level of source texture we want to blit
    //! \param blend_state blend state to use when blitting
    //! \param blend_color blend color
    void blit(
        const Blit_Id id,
        nvrhi::ICommandList* command_list,
        nvrhi::ITexture* source_texture,
        nvrhi::IFramebuffer* dest_frame_buffer,
        const uint32_t source_array_slice = 0,
        const uint32_t source_mip_level = 0,
        const nvrhi::BlendState::RenderTarget& blend_state = detail::DEFAULT_BLEND_STATE,
        const nvrhi::Color& blend_color = nvrhi::Color(0.0)
    ) {
       const nvrhi::TextureDesc& source_texture_desc = source_texture->getDesc();

        if (!detail::is_supported_blit_dimension(source_texture_desc.dimension)) [[unlikely]] {
            throw std::runtime_error("Attempted to blit a texture with unsupported dimensions");
        }

        blit_instance_t& instance = _instances[static_cast<size_t>(id)];

        if (!instance) {
            const bool is_array = detail::is_texture_array(source_texture_desc.dimension);

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
                    nvrhi::BindingSetItem::Texture_SRV(0, source_texture, nvrhi::Format::UNKNOWN, source_sub_resource, source_dimension),
                    nvrhi::BindingSetItem::Sampler(0, _linear_sampler)
                };
            }
            instance.binding_set = _device->createBindingSet(binding_set_desc, _blit_layout);

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
            instance.pipeline = _device->createGraphicsPipeline(desc, dest_frame_buffer);
        }

        const nvrhi::FramebufferInfoEx& frame_buffer_info = dest_frame_buffer->getFramebufferInfo();
        const nvrhi::Viewport viewport = nvrhi::Viewport(
            static_cast<float>(frame_buffer_info.width),
            static_cast<float>(frame_buffer_info.height)
        );

        nvrhi::GraphicsState state;
        {
            state.pipeline              = instance.pipeline;
            state.framebuffer           = dest_frame_buffer;
            state.bindings              = { instance.binding_set };
            state.blendConstantColor    = blend_color;

            state.viewport.addViewportAndScissorRect(viewport);
        }
        command_list->setGraphicsState(state);

        nvrhi::DrawArguments args;
        {
            args.instanceCount = 1;
            args.vertexCount = 4;
        }
        command_list->draw(args);
    }

private:
    nvrhi::IDevice* _device;
    nvrhi::ShaderHandle _rect_shader;

    nvrhi::ShaderHandle _blit_shader;
    nvrhi::ShaderHandle _blit_shader_array;

    nvrhi::SamplerHandle _linear_sampler;
    nvrhi::BindingLayoutHandle _blit_layout;

    std::array<blit_instance_t, Num_Ids> _instances;
};

}