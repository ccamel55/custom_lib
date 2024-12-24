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

        result.backing_vertices.reserve(result.vertex_buffer.size());
        result.backing_indices.reserve(result.index_buffer.size());

        return result;
    }

    //! Add backing data into device buffers
    //! \param command_list command list that will perform thew write
    //! \returns number of indices that where updated
    size_t update_buffers(nvrhi::ICommandList* command_list) {

        if (backing_vertices.empty() || backing_indices.empty()) {
            return 0;
        }

        vertex_buffer.write(command_list, backing_vertices.data(), backing_vertices.size() * sizeof(vertex_t));
        index_buffer.write(command_list, backing_indices.data(), backing_indices.size() * sizeof(index_t));

        const size_t num_indices_updated = backing_indices.size();

        backing_vertices.clear();
        backing_indices.clear();

        return num_indices_updated;
    }

    buffer_object_t vertex_buffer;
    buffer_object_t index_buffer;

    std::vector<vertex> backing_vertices;
    std::vector<index> backing_indices;

};

}