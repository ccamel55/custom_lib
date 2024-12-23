#include <module_render/types/buffer_object.hpp>

using namespace lib::render;

buffer_object_t buffer_object_t::create(
    const nvrhi::BufferHandle buffer,
    const nvrhi::InputLayoutHandle input,
    const size_t offset,
    const size_t size
) {

}


buffer_object_t buffer_object_t::create(
    const buffer_object_t& buffer,
    const size_t offset,
    const size_t size
) {

}