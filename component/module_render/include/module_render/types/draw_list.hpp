#pragma once

#include <module_render/types/buffer_object.hpp>

namespace lib::render {

template<typename vertex, typename index>
struct draw_list_t {

    using vertex_t  = vertex;
    using index_t   = index;

    //! Get nvrhi format for current index type
    [[nodiscard]] constexpr nvrhi::Format index_format() const {
        if constexpr (std::is_same_v<index_t, uint32_t>) {
            return nvrhi::Format::R32_UINT;
        }
        return nvrhi::Format::UNKNOWN;
    }

    //! Create a new draw list from existing vertex and index buffer
    //! \param vertex_buffer vertex buffer to write to
    //! \param index_buffer index buffer to write to
    [[nodiscard]] static draw_list_t create(
        const buffer_object_t& vertex_buffer,
        const buffer_object_t& index_buffer
    ) {
        draw_list_t result;

        result.vertex_buffer    = vertex_buffer;
        result.index_buffer     = index_buffer;

        result.backing_vertices.resize(result.vertex_buffer.size());
        result.backing_indices.resize(result.index_buffer.size());

        result.num_vertices = 0;
        result.num_indices = 0;

        return result;
    }

    //! Add a certain number of empty vertices to the draw list
    //! \param first_vertex_index index for first vertex being emplaced
    //! \param amount the number of vertices that should be added
    //! \returns std::span representing the newly allocated memory for vertex data
    [[nodiscard]] std::span<vertex_t> emplace_vertices(size_t& first_vertex_index, const size_t amount) {
        if (num_vertices + amount > vertex_buffer.size()) [[unlikely]] {
            throw std::runtime_error("Could not add vertices, resulting buffer will exceed bounds");
        }

        first_vertex_index = num_vertices;
        num_vertices += amount;

        return std::span<vertex_t>(
            backing_vertices.begin() + first_vertex_index,
            backing_vertices.begin() + num_vertices
        );
    }

    //! Add a certain number of empty indices to the draw list
    //! \param amount the number of indices that should be added
    //! \returns std::span representing the newly allocated memory for index data
    [[nodiscard]] std::span<index_t> emplace_indices(const size_t amount) {
        if (num_indices + amount > index_buffer.size()) [[unlikely]] {
            throw std::runtime_error("Could not add indices, resulting buffer will exceed bounds");
        }

        const size_t first_index_index = num_indices;
        num_indices += amount;

        return std::span<index_t>(
            backing_indices.begin() + first_index_index,
            backing_indices.begin() + num_indices
        );
    }

    //! Add backing data into device buffers
    //! \param command_list command list that will perform thew write
    //! \returns number of indices that where updated
    size_t update_buffers(nvrhi::ICommandList* command_list) {

        if (num_vertices == 0 || num_indices == 0) {
            return 0;
        }

        vertex_buffer.write(command_list, backing_vertices.data(), num_vertices * sizeof(vertex_t));
        index_buffer.write(command_list, backing_indices.data(), num_indices * sizeof(index_t));

        const size_t num_indices_updated = num_indices;

        num_vertices    = 0;
        num_indices     = 0;

        return num_indices_updated;
    }

    buffer_object_t vertex_buffer;
    buffer_object_t index_buffer;

    size_t num_vertices = 0;
    size_t num_indices  = 0;

private:
    // NOTE: DO NOT USE std::vector functions other than `.resize()` and `.size()`.
    std::vector<vertex> backing_vertices;
    std::vector<index> backing_indices;

};

}