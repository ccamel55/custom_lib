#include <module_render/types/buffer_object.hpp>

using namespace lib::render;

buffer_object_t::~buffer_object_t() {
    if (_buffer_ptr != nullptr) {
        unmap();
    }
}

buffer_object_t buffer_object_t::create(
    nvrhi::IDevice* device,
    const nvrhi::BufferDesc& buffer_desc
) {
    return {
        device,
        device->createBuffer(buffer_desc),
        0,
        buffer_desc.byteSize
    };
}

std::expected<buffer_object_t, std::string> buffer_object_t::create(
    const buffer_object_t& buffer,
    const size_t offset,
    const size_t size
) {
    if (size == 0) [[unlikely]] {
        return std::unexpected("size must not be 0");
    }

    if (offset + size > buffer._size) [[unlikely]] {
        return std::unexpected("new buffer exceeds referenced buffer bounds");
    }

    return buffer_object_t(
        buffer._device,
        buffer._buffer,
        buffer._offset + offset,
        size
    );
}

//
// -----------------------------------------------------------------------
//

void buffer_object_t::write(
    nvrhi::ICommandList* command_list,
    const void* data,
    const size_t data_size_bytes,
    const size_t offset_bytes
) const {

    if (_buffer_ptr != nullptr) {
        // Todo: implement vectorised copy
        std::memcpy(static_cast<uint8_t*>(_buffer_ptr) + _offset + offset_bytes, data, data_size_bytes);
    }
    else {
        command_list->writeBuffer(_buffer, data, data_size_bytes, _offset + offset_bytes);
    }
}

void buffer_object_t::map(const nvrhi::CpuAccessMode map_mode) {

    assert(_buffer_ptr == nullptr);
    assert(_cpu_access != nvrhi::CpuAccessMode::None);

    _buffer_ptr = _device->mapBuffer(_buffer, map_mode);
}

void buffer_object_t::unmap() {

    assert(_buffer_ptr != nullptr);
    assert(_cpu_access != nvrhi::CpuAccessMode::None);

    _buffer_ptr = nullptr;
    _device->unmapBuffer(_buffer);
}