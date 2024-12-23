#include <module_render/util/BufferObject.hpp>

using namespace lib::render;

BufferObject::BufferObject(
    nvrhi::IDevice* device,
    std::string name,
    const size_t size_bytes,
    const BufferUsage usage,
    const BufferType type
)
    : _device(device)
    , _name(std::move(name))
    , _size(size_bytes)
    , _usage(usage)
    , _type(type) {

    assert(_device);
    assert(_size > 0);

    nvrhi::BufferDesc desc;

    desc.byteSize       = _size;
    desc.initialState   = nvrhi::ResourceStates::CopyDest;

    switch (_usage) {
        case BufferUsage::Static:
            _name = _name + "-static";
            desc.debugName          = _name;
            desc.keepInitialState   = true;
            break;
        case BufferUsage::Dynamic:
            _name = _name + "-mapped";
            desc.debugName = _name;
            desc.cpuAccess = nvrhi::CpuAccessMode::Write;
            break;
    }

    switch (_type) {
        case BufferType::Vertex:
            desc.isVertexBuffer = true;
            break;
        case BufferType::Index:
            desc.isIndexBuffer = true;
            break;
        case BufferType::Constant:
            desc.isConstantBuffer = true;
            break;
    }

    _buffer = _device->createBuffer(desc);

    assert(_buffer);
}

BufferObject::~BufferObject() {
    if (_mapped) {
        unmap();
    }
}

void BufferObject::map(const nvrhi::CpuAccessMode map_mode) {

    assert(_usage == BufferUsage::Dynamic);
    assert(_mapped == false);

    _mapped = true;
    _buffer_mapped = _device->mapBuffer(_buffer, map_mode);
}

void BufferObject::unmap() {

    assert(_usage == BufferUsage::Dynamic);
    assert(_mapped == true);

    _mapped = false;
    _buffer_mapped = nullptr;

    _device->unmapBuffer(_buffer);
}

void BufferObject::update(
    nvrhi::ICommandList* command_list,
    const void* data,
    const size_t size_bytes,
    const size_t offset_bytes
) const {

    assert(command_list);
    assert(data);
    assert(size_bytes > 0);

    if (_usage == BufferUsage::Dynamic) {
        assert(_mapped);
        assert(_buffer_mapped);

        // Write into CPU memory since it's mapped
        // Todo: implement SSE vectorised copy
        std::memcpy(_buffer_mapped + offset_bytes, data, size_bytes);
    }
    else {
        command_list->writeBuffer(_buffer, data, size_bytes, offset_bytes);
    }
}

