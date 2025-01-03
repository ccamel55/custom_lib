#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_render/types/hlsl_alias.hpp>
#include <module_render/util/ShaderFactory.hpp>

#include <module_render/shaders/types/blit_cb.h>

#include <unordered_map>

namespace lib::render {
namespace detail {

struct binding_set_desc_key {
    nvrhi::BindingSetDesc description;

    bool operator==(const binding_set_desc_key& other) const {
        return description == other.description;

    }
    bool operator!=(const binding_set_desc_key& other) const {
        return !( *this == other );
    }

    struct hash {
        size_t operator ()(const binding_set_desc_key& desc) const
        {
            size_t hash = 0;
            hash_combine(hash, desc.description);
            return hash;
        }
    };
};

struct pipeline_desc_key {
    uint32_t width  = 0;
    uint32_t height = 0;
    nvrhi::FramebufferInfo frame_buffer_info;
    nvrhi::BlendState::RenderTarget blend_state;
    bool is_array_shader = false;

    bool operator==(const pipeline_desc_key& other) const {
        return frame_buffer_info == other.frame_buffer_info
            && blend_state == other.blend_state
            && is_array_shader == other.is_array_shader;

    }
    bool operator!=(const pipeline_desc_key& other) const {
        return !( *this == other );
    }

    struct hash  {
        size_t operator()(const pipeline_desc_key& desc) const {
            size_t hash = 0;
            nvrhi::hash_combine(hash, desc.width);
            nvrhi::hash_combine(hash, desc.height);
            nvrhi::hash_combine(hash, desc.frame_buffer_info);
            nvrhi::hash_combine(hash, desc.blend_state);
            nvrhi::hash_combine(hash, desc.is_array_shader);
            return hash;
        }
    };
};

constexpr nvrhi::BlendState::RenderTarget DEFAULT_BLEND_STATE = nvrhi::BlendState::RenderTarget()
    .enableBlend()
    .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
    .setSrcBlendAlpha(nvrhi::BlendFactor::One)
    .setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha)
    .setDestBlendAlpha(nvrhi::BlendFactor::OneMinusSrcAlpha)
    .setBlendOp(nvrhi::BlendOp::Add);

}

class TextureBlit {
public:
    TextureBlit(
        nvrhi::IDevice* device,
        const std::unique_ptr<ShaderFactory>& shader_factory
    );

    //! Reset pipeline and binding set so we can rebind to new frame buffer texture
    void back_buffer_resizing();

    //! Blit texture from given parameters
    //! \param command_list command list that will execute the blit
    //! \param source_texture source texture that we want to copy
    //! \param dest_frame_buffer frame buffer we will use to write the source texture
    //! \param source_array_slice array slice/index of texture. Only used for array textures
    //! \param source_mip_level mipmap level of source texture we want to blit
    //! \param blend_state blend state to use when blitting
    //! \param blend_color blend color
    void blit(
        nvrhi::ICommandList* command_list,
        nvrhi::ITexture* source_texture,
        nvrhi::IFramebuffer* dest_frame_buffer,
        uint32_t source_array_slice = 0,
        uint32_t source_mip_level = 0,
        const nvrhi::BlendState::RenderTarget& blend_state = detail::DEFAULT_BLEND_STATE,
        const nvrhi::Color& blend_color = nvrhi::Color(0.0)
    );

private:
    nvrhi::IDevice* _device;
    nvrhi::ShaderHandle _rect_shader;

    nvrhi::ShaderHandle _blit_shader;
    nvrhi::ShaderHandle _blit_shader_array;

    nvrhi::SamplerHandle _linear_sampler;
    nvrhi::BindingLayoutHandle _blit_layout;

    std::unordered_map<detail::binding_set_desc_key, nvrhi::BindingSetHandle, detail::binding_set_desc_key::hash> _binding_set;
    std::unordered_map<detail::pipeline_desc_key, nvrhi::GraphicsPipelineHandle, detail::pipeline_desc_key::hash> _pipeline;
};

}