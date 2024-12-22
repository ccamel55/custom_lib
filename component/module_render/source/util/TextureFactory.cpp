#include <module_render/util/TextureFactory.hpp>
#include <module_system/filesystem.hpp>

using namespace lib::render;

TextureFactory::TextureFactory(const nvrhi::DeviceHandle& device)
    : _device(device) {

}