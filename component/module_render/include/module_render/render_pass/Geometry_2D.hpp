#pragma once

#include <dep_glm/glm.hpp>

#include <module_render/RenderPass.hpp>
#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureFactory.hpp>

namespace lib::render {
namespace detail {

constexpr size_t MAX_VERTICES   = 1024;
constexpr size_t MAX_INDICES    = MAX_VERTICES * 3;

struct vertex_t {
    glm::float32_t position[3];
    glm::float32_t uv[2];
    glm::float32_t color[4];
};

using index_t = uint32_t;

struct constant_buffer_t {
    glm::f32mat4x4 view_projection;
    float padding[4 * 4 * 3] = {};
};

static_assert(sizeof(constant_buffer_t) == nvrhi::c_ConstantBufferOffsetSizeAlignment, "sizeof(ConstantBufferEntry) must be 256 bytes");
}

class Geometry_2D final : public RenderPass {
public:
    explicit Geometry_2D(Device_Common* backend);

    void init(
        const std::filesystem::path& shader_folder,
        const std::unique_ptr<ShaderFactory>& shader_factory,
        const std::unique_ptr<TextureFactory>& texture_factory
    );

    void update_frame(const FrameInterval& interval) override;
    void render(nvrhi::IFramebuffer* frame_buffer) override;
    void back_buffer_resizing() override;
    void back_buffer_resized(const point2Di& size) override;

private:
    nvrhi::ShaderHandle _vertex_shader;
    nvrhi::ShaderHandle _pixel_shader;

    nvrhi::BufferHandle _vertex_buffer;
    nvrhi::BufferHandle _index_buffer;
    nvrhi::BufferHandle _constant_buffer;

    nvrhi::InputLayoutHandle _vertex_layout;

    nvrhi::BindingLayoutHandle _binding_layout;
    nvrhi::BindingSetHandle _binding_set;

    nvrhi::CommandListHandle _command_list;
    nvrhi::GraphicsPipelineHandle _pipeline;

    std::array<detail::vertex_t, detail::MAX_VERTICES> _vertices;
    std::array<detail::index_t, detail::MAX_INDICES> _indices;
    detail::constant_buffer_t _constants;
};

}
