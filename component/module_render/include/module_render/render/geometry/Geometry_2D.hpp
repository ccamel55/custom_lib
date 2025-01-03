#pragma once

#include <module_core/NoCopy.hpp>

#include <module_hashing/fnv1a_32.hpp>

// Must be before all types, this pre-declares some hlsl types
#include <module_render/types/hlsl_alias.hpp>

#include <module_render/backend/Device_Common.hpp>
#include <module_render/render/geometry/types/vertex.hpp>

#include <module_render/shaders/types/geometry_cb.h>
#include <module_render/types/buffer_object.hpp>

#include <module_render/util/Pipeline.hpp>
#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureBlit.hpp>
#include <module_render/util/TextureFactory.hpp>

static_assert(
    sizeof(constant_buffer_t) % nvrhi::c_ConstantBufferOffsetSizeAlignment == 0,
    "sizeof(constant_buffer_t) must be 256 bytes"
);

namespace lib::render {

enum class Pipeline_Id: uint32_t {
    Geometry_Texture,

    // Must always be last
    Num_Pipeline_Id
};

using Texture_Id = hashing::fnv1a_32_t;

using Geometry_Pipeline     = Pipeline<Pipeline_Id, static_cast<size_t>(Pipeline_Id::Num_Pipeline_Id)>;
using Geometry_BindingSet   = std::unordered_map<detail::binding_set_desc_key, nvrhi::BindingSetHandle, detail::binding_set_desc_key::hash>;

constexpr Texture_Id TEXTURE_WHITE = hashing::fnv1a_32("default");

struct draw_command_t {

    draw_command_t(
        const Texture_Id texture,
        const Pipeline_Id pipeline,
        const size_t offset = 0
    )
        : texture(texture)
        , pipeline(pipeline)
        , offset(offset)
        , count(0) {

    }

    [[nodiscard]] bool compatible(const Texture_Id o_texture, const Pipeline_Id o_pipeline) const {
        return texture == o_texture
            && pipeline == o_pipeline;
    }

    Texture_Id texture;
    Pipeline_Id pipeline;
    uint32_t offset;
    uint32_t count;
};

struct draw_list_t {

    draw_list_t(
        const size_t max_vertices,
        const size_t max_indices
    )
        : backing_vertices(max_vertices)
        , backing_indices(max_indices) {

        draw_commands.reserve(512);
    }

    //! Called before any drawing occurs. This will create a new draw command if needed
    //! \param texture texture ID that will be drawed
    //! \param pipeline pipeline ID that will be used to draw
    void prepare_draw(const Texture_Id texture, const Pipeline_Id pipeline) {
        if (draw_commands.empty()) {
            draw_commands.emplace_back(texture, pipeline);
        }
        else if (!draw_commands.back().compatible(texture, pipeline)) {
            draw_commands.emplace_back(texture, pipeline, num_indices);
        }
    }

    //! Add a certain number of empty vertices to the draw list
    //! \param first_vertex_index index for first vertex being emplaced
    //! \param amount the number of vertices that should be added
    //! \returns std::span representing the newly allocated memory for vertex data
    [[nodiscard]] std::span<detail::vertex_t> emplace_vertices(size_t& first_vertex_index, const size_t amount) {
        if (num_vertices + amount > backing_vertices.size()) [[unlikely]] {
            throw std::runtime_error("Could not add vertices, resulting buffer will exceed bounds");
        }

        first_vertex_index = num_vertices;
        num_vertices += amount;

        return {
            backing_vertices.begin() + static_cast<long>(first_vertex_index),
            backing_vertices.begin() + static_cast<long>(num_vertices)
        };
    }

    //! Add a certain number of empty indices to the draw list
    //! \param amount the number of indices that should be added
    //! \returns std::span representing the newly allocated memory for index data
    [[nodiscard]] std::span<detail::index_t> emplace_indices(const size_t amount) {
        if (num_indices + amount > backing_indices.size()) [[unlikely]] {
            throw std::runtime_error("Could not add indices, resulting buffer will exceed bounds");
        }

        const size_t first_index_index = num_indices;

        draw_commands.back().count += amount;
        num_indices += amount;

        return {
            backing_indices.begin() + static_cast<long>(first_index_index),
            backing_indices.begin() + static_cast<long>(num_indices)
        };
    }

    [[nodiscard]] size_t max_vertices() const {
        return backing_vertices.size();
    }

    [[nodiscard]] size_t max_indices() const {
        return backing_indices.size();
    }

    size_t num_vertices = 0;
    size_t num_indices  = 0;

    std::vector<draw_command_t> draw_commands;

    // NOTE: DO NOT USE std::vector functions other than `.resize()` and `.size()`.
    std::vector<detail::vertex_t> backing_vertices;
    std::vector<detail::index_t> backing_indices;
};

class Geometry_2D final : public NoCopy {
public:
    explicit Geometry_2D(
        const nvrhi::DeviceHandle& device,
        const std::unique_ptr<ShaderFactory>& shader_factory,
        const std::unique_ptr<TextureFactory>& texture_factory
    );

    void draw_geometry(const nvrhi::CommandListHandle& command_list, nvrhi::IFramebuffer* frame_buffer);

    void back_buffer_resizing();
    void back_buffer_resized(const point2Di& size);

public:
    void triangle(const point2Df& pos, const size_t size, const std::array<uint8_t, 4> color = { 255, 255, 255, 255 }) {

        const auto centre_pos = pos;

        _draw.prepare_draw(TEXTURE_WHITE, Pipeline_Id::Geometry_Texture);

        size_t first_vertex_index;
        std::span<detail::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 3);

        vertices[0] = detail::vertex_t(centre_pos.x, centre_pos.y - size / 2, 0.0, 0.5, 0.0, color[0], color[1], color[2], color[3]);
        vertices[1] = detail::vertex_t(centre_pos.x + size / 2, centre_pos.y + size / 2, 0.0, 1.0, 1.0, color[0], color[1], color[2], color[3]);
        vertices[2] = detail::vertex_t(centre_pos.x - size / 2, centre_pos.y + size / 2, 0.0, 0.0, 1.0, color[0], color[1], color[2], color[3]);

        std::span<detail::index_t> indices = _draw.emplace_indices(3);

        indices[0] = first_vertex_index + 0;
        indices[1] = first_vertex_index + 1;
        indices[2] = first_vertex_index + 2;
    }

private:
    nvrhi::DeviceHandle _device;
    nvrhi::BindingLayoutHandle _binding_layout;

    Geometry_Pipeline _pipeline;
    Geometry_BindingSet _binding_set;

    buffer_object_t _vertex_buffer;
    buffer_object_t _index_buffer;
    buffer_object_t _constant_buffer;

    nvrhi::ShaderHandle _vertex_shader;
    nvrhi::ShaderHandle _pixel_shader;
    nvrhi::InputLayoutHandle _vertex_layout;

    draw_list_t _draw;
    nvrhi::SamplerHandle _sampler; // TODO: cache sampler

    std::unordered_map<Texture_Id, nvrhi::TextureHandle> _texture;

    bool _update_constant_buffer    = false;
    bool _update_pipeline           = false;
};

}
