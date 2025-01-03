#define GLFW_INCLUDE_VULKAN
#include <dep_glfw/glfw.hpp>

#include <module_core/type/point/point2D.hpp>
#include <module_logger/Logger.hpp>
#include <module_logger/ScopedLog.hpp>
#include <module_logger/handler/Std_LogHandler.hpp>

#include <module_render/Render.hpp>
#include <module_render/backend/Device_Vulkan.hpp>
#include <module_render/geometry/Geometry_2D.hpp>
#include <module_render/pass/BasicTriangle.hpp>
#include <module_render/util/FrameBuffer.hpp>
#include <module_render/util/Image.hpp>
#include <module_render/util/TextureBlit.hpp>

#include <module_system/filesystem.hpp>

using namespace lib;

namespace {
// Glfw window handle
GLFWwindow* window = nullptr;

const std::filesystem::path EXE_PATH = system::get_executable_path().value().parent_path();

std::unique_ptr<render::Render> RENDER = nullptr;
std::shared_ptr<logger::Logger> LOGGER = nullptr;

namespace callback {
void window_size_callback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] const int width,
    [[maybe_unused]] const int height) {

}

void key_callback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] const int key,
    [[maybe_unused]] const int scancode,
    [[maybe_unused]] const int action,
    [[maybe_unused]] const int mods
) {

}

void scroll_callback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] const double offset_x,
    [[maybe_unused]] const double offset_y
) {

}

void cursor_position_callback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] const double pos_x,
    [[maybe_unused]] const double pos_y
) {

}

void mouse_button_callback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] const int button,
    [[maybe_unused]] const int action,
    [[maybe_unused]] const int mods
) {

}
}

namespace init {
#ifdef CAMEL_NVRHI_VULKAN
// Setup things and populate settings for vulkan render instance
[[nodiscard]] std::expected<render::device_settings_t, std::string> prepare_api(GLFWwindow* window) {

    if (!glfwVulkanSupported()) {
        return std::unexpected("Vulkan not supported");
    }

    render::device_settings_t settings = {};
    {
        #if _DEBUG
        settings.debug          = true;
        settings.validation     = true;
        #else
        settings.debug          = false;
        settings.validation     = false;
        #endif

        settings.compute_queue  = false;
        settings.copy_queue     = false;
        settings.vsync          = false;

        settings.back_buffer_size   = {};
        settings.vulkan             = {};
    }

    glfwGetWindowSize(window, &settings.back_buffer_size.x, &settings.back_buffer_size.y);

    // Get required extensions from GLFW
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    for (uint32_t i = 0; i < glfw_extension_count; ++i) {
        settings.vulkan.extra_extension_enabled.instance.emplace(glfw_extensions[i]);
    }

    // Set callbacks using GLFW
    settings.vulkan.create_window_surface = [window](void* _param) -> int {
        const auto param = static_cast<render::vk_create_surface_callback_t*>(_param);
        return glfwCreateWindowSurface(
            *param->instance,
            window, nullptr,
            reinterpret_cast<VkSurfaceKHR*>(param->surface)
        );
    };

    settings.vulkan.get_physical_device_presentation_support = [](void* _param) -> int {
        const auto param = static_cast<render::vk_get_physical_device_support_t*>(_param);
        return glfwGetPhysicalDevicePresentationSupport(
            *param->instance,
            *param->physical_device,
            param->queue_family
        );
    };

    return settings;
}
#endif

#ifdef CAMEL_NVRHI_DX_11
[[nodiscard]] std::expected<render::device_settings_t, std::string> prepare_api() {
    return std::unexpected("DX11 not implemented");
}
#endif

#ifdef CAMEL_NVRHI_DX_12
[[nodiscard]] std::expected<render::device_settings_t, std::string> prepare_api() {
    return std::unexpected("DX12 not implemented");
}
#endif
}

enum class FrameBuffer_Id: uint32_t {
    Geometry_2d,

    // Must always be last
    Num_FrameBuffer_Id
};

enum class Image_Id: uint32_t {
    Geometry_2d_ColorTarget,

    // Must always be last
    Num_Image_Id
};

using FrameBuffer   = render::FrameBuffer<FrameBuffer_Id, static_cast<size_t>(FrameBuffer_Id::Num_FrameBuffer_Id)>;
using Image         = render::Image<Image_Id, static_cast<size_t>(Image_Id::Num_Image_Id)>;

class ExamplePass final : public render::RenderPass {
public:
    ExamplePass(
        const nvrhi::DeviceHandle& device,
        const std::shared_ptr<render::ShaderFactory>& shader_factory,
        const std::shared_ptr<render::TextureFactory>& texture_factory
    )
        : RenderPass(device)
        , _geometry_2d(
            _device,
            shader_factory,
            texture_factory
        )
        , _blit(device, shader_factory)
        , _image(device)
        , _frame_buffer(device) {

        _command_list = _device->createCommandList();

        _cat_image = _geometry_2d.add_texture("cat.jpg").value();
    }

    void update_frame(const render::FrameInterval& interval) override {
        _geometry_2d.d_texture({200, 200}, {200, 200}, _cat_image);
        _geometry_2d.d_line({200, 200}, {400, 400}, { 0, 255, 255, 100 }, 10.f);
    }

    void render(nvrhi::IFramebuffer* frame_buffer) override {
        _command_list->open();
        {
            _geometry_2d.draw_geometry(_command_list, _frame_buffer[FrameBuffer_Id::Geometry_2d]);
        }
        // This must be called here, it will blit the color target to our currently presented frame buffer
        {
            _blit.blit(_command_list, _image[Image_Id::Geometry_2d_ColorTarget], frame_buffer);
        }
        _command_list->close();
        _device->executeCommandList(_command_list);
    }

    void back_buffer_resizing() override {
        _geometry_2d.back_buffer_resizing();
        _blit.back_buffer_resizing();
    }

    void back_buffer_resized(const point2Di& size) override {
        _image.back_buffer_resized([&](auto& image) {
            image[static_cast<size_t>(Image_Id::Geometry_2d_ColorTarget)] = _device->createTexture(
                nvrhi::TextureDesc()
                    .setDebugName("ColorTarget")
                    .setFormat(nvrhi::Format::SBGRA8_UNORM)
                    .setWidth(std::max(size.x, 1))
                    .setHeight(std::max(size.y, 1))
                    .setIsRenderTarget(true)
                    .setKeepInitialState(true)
                    .setInitialState(nvrhi::ResourceStates::RenderTarget)
            );
        });

        _frame_buffer.back_buffer_resized([&](auto& frame_buffer) {
            frame_buffer[static_cast<size_t>(FrameBuffer_Id::Geometry_2d)] = _device->createFramebuffer(
                nvrhi::FramebufferDesc()
                    .addColorAttachment(_image[Image_Id::Geometry_2d_ColorTarget])
            );
        });
    }

private:
    render::Geometry_2D _geometry_2d;
    render::TextureBlit _blit;
    Image _image;
    FrameBuffer _frame_buffer;

    nvrhi::CommandListHandle _command_list;

    render::geometry::Texture_Id _cat_image;

};
}

int main(
    [[maybe_unused]] int argc,
    [[maybe_unused]] const char * argv[]
) {
    // Create SDK handlers
    LOGGER = std::make_shared<logger::Logger>();
    LOGGER->add_handler(std::make_shared<logger::Std_Handler>(lib::logger::log_level::VERBOSE));

    logger::ScopeLog log(LOGGER, "main");
    log.v("creating GLFW window");

    if (!glfwInit()) {
        log.e("failed to call `glfwInit`");
        return 1;
    }

    // Spawn GLFW with no default graphics API and disable MSAA
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    const point2Di WINDOW_SIZE(1600, 900);

    window = glfwCreateWindow(
        WINDOW_SIZE.x,
        WINDOW_SIZE.y,
        "Test Window",
        nullptr,
        nullptr
    );

    if (!window) {
        log.e("failed to create window");
        return 1;
    }

    const auto cleanup_glf = [&] {
        // Cleanup resources
        glfwDestroyWindow(window);
        glfwTerminate();
    };

    // Bring window to context and disable VSync
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    // Register callbacks
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    {
        glfwSetWindowSizeCallback(window,   callback::window_size_callback);
        glfwSetKeyCallback(window,          callback::key_callback);
        glfwSetScrollCallback(window,       callback::scroll_callback);
        glfwSetCursorPosCallback(window,    callback::cursor_position_callback);
        glfwSetMouseButtonCallback(window,  callback::mouse_button_callback);
    }

    // Read back window size must not be 0, 0
    point2Di window_size(0, 0);
    glfwGetWindowSize(window, &window_size.x, &window_size.y);

    if (window_size.x == 0 && window_size.y == 0) {
        log.e("window size is [0, 0] something went wrong");
        cleanup_glf();
        return 1;
    }

    log.v("starting main loop");

    const auto settings = init::prepare_api(window);

    if (!settings) {
        log.e("could not prepare api: {}", settings.error());
        cleanup_glf();
        return 1;
    }

    RENDER = std::make_unique<render::Render>(LOGGER, settings.value(), render::RenderAPI::Vulkan);

    std::shared_ptr<render::ShaderFactory> SHADER_FACTORY   = nullptr;
    std::shared_ptr<render::TextureFactory> TEXTURE_FACTORY = nullptr;

    SHADER_FACTORY  = std::make_shared<render::ShaderFactory>(
        RENDER->backend()->device_handle(),
        EXE_PATH / "shaders" / shader_type(RENDER->backend()->api())
    );

    TEXTURE_FACTORY = std::make_shared<render::TextureFactory>(
        RENDER->backend()->device_handle(),
        EXE_PATH / "textures"
    );

    const auto triangle_pass    = std::make_unique<render::BasicTriangle>(RENDER->backend()->device_handle(), SHADER_FACTORY);
    const auto example_pass     = std::make_unique<ExamplePass>(RENDER->backend()->device_handle(), SHADER_FACTORY, TEXTURE_FACTORY);

    RENDER->emplace_render_pass_back(triangle_pass.get());
    RENDER->emplace_render_pass_back(example_pass.get());

    // Main window loop
    while (!glfwWindowShouldClose(window)) {

        // Update input
        glfwPollEvents();

        // Draw
        glfwGetWindowSize(window, &window_size.x, &window_size.y);

        RENDER->update_screen_size(window_size);
        RENDER->present();
    }

    log.v("exited main loop");

    return 0;
}
