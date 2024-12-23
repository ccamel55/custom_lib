#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_core/NoCopy.hpp>

namespace lib::render {

enum class BufferUsage {
    Static,
    Dynamic
};

enum class BufferType {
    Vertex,
    Index,
    Constant,
};

// Buffer object
// TODO: support creation of buffer object inside another buffer object
// TODO: permanent buffers with no states

class BufferObject : public NoCopy {
public:
    BufferObject(
        nvrhi::IDevice* device,
        std::string name,
        size_t size_bytes,
        BufferUsage usage,
        BufferType type
    );
    ~BufferObject();

    //! Map a buffer to CPU memory
    //! \note this only works for dynamic buffer usage type
    //! \note buffer must be unmapped before being drawn
    //! \param map_mode CPU access mode
    void map(nvrhi::CpuAccessMode map_mode);

    //! Unmap buffer from CPU memory
    //! \note this only works for dynamic buffer usage type
    //! \note must be done before using buffer on GPU
    void unmap();

    //! Update buffer contents
    //! \note this will copy to CPU memory if dynamic buffer usage
    //! \param command_list nvrhi command list pointer
    //! \param data raw pointer to src data
    //! \param size_bytes size of src data in bytes
    //! \param offset_bytes offset into dst buffer
    void update(
        nvrhi::ICommandList* command_list,
        const void* data,
        size_t size_bytes,
        size_t offset_bytes
    ) const;

    //! Get pointer to created API buffer object
    //! \return pointer to created NVRHI buffer
    [[nodiscard]] nvrhi::IBuffer* buffer() const {
        return _buffer;
    }

    //! Get maximum size of buffer
    //! \return allocated size of buffer object
    [[nodiscard]] size_t size() const {
        return _size;
    }

protected:
    nvrhi::IDevice* _device;
    std::string _name;
    size_t _size;
    BufferUsage _usage;
    BufferType _type;

    nvrhi::BufferHandle _buffer;
    void* _buffer_mapped    = nullptr;
    bool _mapped            = false;

};

}