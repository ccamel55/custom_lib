#pragma once

#include <module_render/geometry/types/vertex.hpp>

#include <optional>

namespace lib::render::geometry {

struct draw_command_t {

    draw_command_t(
        const Texture_Id& texture,
        const Pipeline_Id pipeline,
        const std::optional<nvrhi::Viewport>& viewport,
        const std::optional<nvrhi::Rect>& scissor,
        const size_t offset = 0
    )
        : texture(texture)
        , pipeline(pipeline)
        , viewport(viewport)
        , scissor(scissor)
        , offset(offset)
        , count(0) {

    }

    [[nodiscard]] bool compatible(
        const Texture_Id& o_texture,
        const Pipeline_Id o_pipeline,
        const std::optional<nvrhi::Viewport>& o_viewport,
        const std::optional<nvrhi::Rect>& o_scissor
    ) const {
        return texture == o_texture
            && pipeline == o_pipeline
            && viewport == o_viewport
            && scissor == o_scissor;
    }

    Texture_Id texture;
    Pipeline_Id pipeline;
    std::optional<nvrhi::Viewport> viewport;
    std::optional<nvrhi::Rect> scissor;

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
    //! \param texture texture ID that will be drawn
    //! \param pipeline pipeline ID that will be used to draw
    //! \param viewport viewport, defaults to whole frame buffer
    //! \param scissor scissor, defaults to whole viewport
    void prepare_draw(
        const Texture_Id& texture,
        const Pipeline_Id pipeline,
        const std::optional<nvrhi::Viewport>& viewport = std::nullopt,
        const std::optional<nvrhi::Rect>& scissor = std::nullopt
    ) {
        if (draw_commands.empty() ||
            draw_commands.back().compatible(texture, pipeline, viewport, scissor) == false) {
            draw_commands.emplace_back(texture, pipeline, viewport, scissor, num_indices);
        }
    }

    //! Add a certain number of empty vertices to the draw list
    //! \param first_vertex_index index for first vertex being emplaced
    //! \param amount the number of vertices that should be added
    //! \returns std::span representing the newly allocated memory for vertex data
    [[nodiscard]] std::span<vertex_t> emplace_vertices(size_t& first_vertex_index, const size_t amount) {
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
    [[nodiscard]] std::span<index_t> emplace_indices(const size_t amount) {
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
    std::vector<vertex_t> backing_vertices;
    std::vector<index_t> backing_indices;
};

}