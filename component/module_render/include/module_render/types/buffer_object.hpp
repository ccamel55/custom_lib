#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

// Buffer object that can either be a new buffer from api OR reference to another
// existing buffer handle.
struct buffer_object_t {

    //! Create a buffer object from an existing nvrhi buffer
    //! \param buffer buffer handle
    //! \param input input handle
    //! \param offset offset into original buffer in bytes
    //! \param size size of new buffer object in bytes
    //! \returns new buffer object referencing an existing buffer handle
    [[nodiscard]] static buffer_object_t create(
        nvrhi::BufferHandle buffer,
        nvrhi::InputLayoutHandle input,
        size_t offset,
        size_t size
    );

    //! Create a buffer object from an existing buffer object
    //! \param buffer buffer object
    //! \param offset offset into original buffer in bytes
    //! \param size size of new buffer object in bytes
    //! \returns new buffer object referencing an existing buffer object
    [[nodiscard]] static buffer_object_t create(
        const buffer_object_t& buffer,
        size_t offset,
        size_t size
    );

    std::string name;
    nvrhi::BufferHandle buffer;
    nvrhi::InputLayoutHandle layout;
    size_t offset;
    size_t size;
};

}