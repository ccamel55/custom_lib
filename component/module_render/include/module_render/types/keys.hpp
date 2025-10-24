#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

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