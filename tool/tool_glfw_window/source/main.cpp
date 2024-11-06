#include <iostream>

#include <RenderUser.hpp>

#define GLFW_INCLUDE_VULKAN
#include <dep_glfw/glfw.hpp>

#include <module_core/type/point/point2D.hpp>
#include <module_logger/Logger.hpp>
#include <module_logger/ScopedLog.hpp>
#include <module_logger/handler/Std_LogHandler.hpp>
#include <module_render/backend/Device_Vulkan.hpp>

using namespace lib;

namespace {
// Glfw window handle
GLFWwindow* window = nullptr;

std::unique_ptr<RenderUser> USER = nullptr;

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
        settings.debug          = true;
        settings.validation     = true;
        settings.compute_queue  = false;
        settings.copy_queue     = false;
        settings.vsync          = false;

        settings.starting_size  = {};
        settings.vulkan         = {};
    }

    glfwGetWindowSize(window, &settings.starting_size.x, &settings.starting_size.y);

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

    bool minimised = false;
    const auto settings = init::prepare_api(window);

    if (!settings) {
        log.e("could not prepare api: {}", settings.error());
        cleanup_glf();
        return 1;
    }

    USER = std::make_unique<RenderUser>(LOGGER, settings.value());

    // Main window loop
    while (!glfwWindowShouldClose(window)) {

        // Update input
        glfwPollEvents();

        // Update window size if needed and visible state
        {
            glfwGetWindowSize(window, &window_size.x, &window_size.y);

            if (window_size.x == 0 || window_size.y == 0) {
                if (!minimised) {
                    log.v("window minimised");
                    minimised = true;
                }
            }
            else {
                if (minimised) {
                    log.v("window unminimised");
                    minimised = false;
                }
            }

            USER->update_screen_size(window_size, minimised);
        }

        USER->on_frame(minimised);
    }

    log.v("exited main loop");

    return 0;
}