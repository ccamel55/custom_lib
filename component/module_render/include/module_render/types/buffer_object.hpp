#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <expected>

namespace lib::render {

// enum class BufferUsage {
//     Static,
//     Dynamic
// };

// Buffer object that can either be a new buffer from api OR reference to another
// existing buffer handle.
class buffer_object_t {
public:
    buffer_object_t() = default;
    ~buffer_object_t();

    buffer_object_t(
        nvrhi::IDevice* device,
        nvrhi::BufferHandle buffer,
        const size_t offset,
        const size_t size
    )
        : _device(device)
        , _buffer(std::move(buffer))
        , _offset(offset)
        , _size(size) {

    }

    //! Create a new buffer from a buffer description
    //! \param device device ptr
    //! \param buffer_desc buffer creation description
    //! \returns new buffer object referencing an existing buffer handle
    [[nodiscard]] static buffer_object_t create(
        nvrhi::IDevice* device,
        const nvrhi::BufferDesc& buffer_desc
    );

    //! Create a buffer object from an existing buffer object
    //! \param buffer buffer object
    //! \param offset offset into original buffer in bytes
    //! \param size size of new buffer object in bytes
    //! \returns new buffer object referencing an existing buffer object
    [[nodiscard]] static std::expected<buffer_object_t, std::string> create(
        const buffer_object_t& buffer,
        size_t offset,
        size_t size
    );

    //! Write data into vertex buffer
    //! \param command_list pointer to command list
    //! \param data void pointer to src data
    //! \param data_size_bytes size of src in bytes
    //! \param offset_bytes where to write data in current buffer object
    void write(
        nvrhi::ICommandList* command_list,
        const void* data,
        size_t data_size_bytes,
        size_t offset_bytes = 0
    ) const;

    //! Map a buffer to CPU memory
    //! \note this function underlying buffer is mappable
    //! \note buffer must be unmapped before being drawn
    //! \param map_mode CPU access mode
    void map(nvrhi::CpuAccessMode map_mode);

    //! Unmap buffer from CPU memory
    //! \note this function underlying buffer is mappable
    //! \note must be done before using buffer on GPU
    void unmap();

    //! Get original buffer pointer
    //! \returns raw pointer to original buffer
    [[nodiscard]] nvrhi::IBuffer* buffer() const {
        return _buffer;
    }

    //! Get offset into original buffer
    //! \returns offset into original buffer in bytes
    [[nodiscard]] size_t offset() const {
        return _offset;
    }

    //! Get size of current buffer
    //! \returns size current buffer
    [[nodiscard]] size_t size() const {
        return _size;
    }

private:
    nvrhi::IDevice* _device             = nullptr;
    nvrhi::BufferHandle _buffer         = nullptr;
    size_t _offset                      = 0;
    size_t _size                        = 0;
    void* _buffer_ptr                   = nullptr;
    nvrhi::CpuAccessMode _cpu_access    = nvrhi::CpuAccessMode::None;
};

}