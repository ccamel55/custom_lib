#define GLFW_INCLUDE_VULKAN
#include <dep_glfw/glfw.hpp>

#include <module_core/type/point/point2D.hpp>

#include <module_gui/Gui.hpp>

#include <module_input/Input.hpp>

#include <module_logger/Logger.hpp>
#include <module_logger/ScopedLog.hpp>
#include <module_logger/handler/Std_LogHandler.hpp>

#include <module_render/Render.hpp>
#include <module_render/backend/Device_Vulkan.hpp>
#include <module_render/geometry/Geometry_2D.hpp>
#include <module_render/pass/BasicTriangle.hpp>
#include <module_render/util/FontFactory.hpp>
#include <module_render/util/FrameBuffer.hpp>
#include <module_render/util/Image.hpp>
#include <module_render/util/TextureBlit.hpp>

#include <module_system/filesystem.hpp>

using namespace lib;

namespace {
// Glfw window handle
GLFWwindow* window = nullptr;

std::unique_ptr<input::Input> INPUTS     = nullptr;
std::unique_ptr<render::Render> RENDER  = nullptr;
std::shared_ptr<logger::Logger> LOGGER  = nullptr;

const std::filesystem::path EXE_PATH = system::get_executable_path().value().parent_path();

namespace callback {
void key_callback(
    [[maybe_unused]] GLFWwindow* window,
    const int key,
    [[maybe_unused]] const int scancode,
    const int action,
    [[maybe_unused]] const int mods
) {
    switch (key) {
        case GLFW_KEY_TAB:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::TAB, action == GLFW_PRESS);
            break;
        case GLFW_KEY_LEFT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::LEFT_ARROW, action == GLFW_PRESS);
            break;
        case GLFW_KEY_RIGHT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::RIGHT_ARROW, action == GLFW_PRESS);
            break;
        case GLFW_KEY_UP:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::UP_ARROW, action == GLFW_PRESS);
            break;
        case GLFW_KEY_DOWN:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::DOWN_ARROW, action == GLFW_PRESS);
            break;
        case GLFW_KEY_PAGE_UP:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::PAGE_UP, action == GLFW_PRESS);
            break;
        case GLFW_KEY_PAGE_DOWN:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::PAGE_DOWN, action == GLFW_PRESS);
            break;
        case GLFW_KEY_HOME:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::HOME, action == GLFW_PRESS);
            break;
        case GLFW_KEY_END:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::END, action == GLFW_PRESS);
            break;
        case GLFW_KEY_INSERT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::INSERT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_DELETE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::DELETE_KEY, action == GLFW_PRESS);
            break;
        case GLFW_KEY_BACKSPACE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::BACKSPACE, action == GLFW_PRESS);
            break;
        case GLFW_KEY_SPACE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::SPACE, action == GLFW_PRESS);
            break;
        case GLFW_KEY_ENTER:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::ENTER, action == GLFW_PRESS);
            break;
        case GLFW_KEY_ESCAPE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::ESCAPE, action == GLFW_PRESS);
            break;
        case GLFW_KEY_APOSTROPHE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::APOSTROPHE, action == GLFW_PRESS);
            break;
        case GLFW_KEY_COMMA:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::COMMA, action == GLFW_PRESS);
            break;
        case GLFW_KEY_MINUS:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::MINUS, action == GLFW_PRESS);
            break;
        case GLFW_KEY_PERIOD:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::PERIOD, action == GLFW_PRESS);
            break;
        case GLFW_KEY_SLASH:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::SLASH, action == GLFW_PRESS);
            break;
        case GLFW_KEY_SEMICOLON:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::SEMICOLON, action == GLFW_PRESS);
            break;
        case GLFW_KEY_EQUAL:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::EQUAL, action == GLFW_PRESS);
            break;
        case GLFW_KEY_LEFT_BRACKET:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::LEFT_BRACKET, action == GLFW_PRESS);
            break;
        case GLFW_KEY_BACKSLASH:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::BACKSLASH, action == GLFW_PRESS);
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::RIGHT_BRACKET, action == GLFW_PRESS);
            break;
        case GLFW_KEY_GRAVE_ACCENT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::GRAVE_ACCENT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_CAPS_LOCK:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::CAPSLOCK, action == GLFW_PRESS);
            break;
        case GLFW_KEY_SCROLL_LOCK:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::SCROLL_LOCK, action == GLFW_PRESS);
            break;
        case GLFW_KEY_NUM_LOCK:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_LOCK, action == GLFW_PRESS);
            break;
        case GLFW_KEY_PRINT_SCREEN:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::PRINT_SCREEN, action == GLFW_PRESS);
            break;
        case GLFW_KEY_PAUSE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::PAUSE, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_0:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD0, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_1:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD1, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_2:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD2, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_3:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD3, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_4:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD4, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_5:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD5, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_6:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD6, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_7:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD7, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_8:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD8, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_9:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD9, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_DECIMAL:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_DECIMAL, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_DIVIDE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_DIVIDE, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_MULTIPLY:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_MULTIPLY, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_SUBTRACT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_SUBTRACT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_ADD:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_ADD, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_ENTER:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_ENTER, action == GLFW_PRESS);
            break;
        case GLFW_KEY_KP_EQUAL:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::KEYPAD_EQUAL, action == GLFW_PRESS);
            break;
        case GLFW_KEY_LEFT_SHIFT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::LEFT_SHIFT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_LEFT_CONTROL:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::LEFT_CTRL, action == GLFW_PRESS);
            break;
        case GLFW_KEY_LEFT_ALT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::LEFT_ALT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_LEFT_SUPER:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::LEFT_SUPER, action == GLFW_PRESS);
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::RIGHT_SHIFT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::RIGHT_CTRL, action == GLFW_PRESS);
            break;
        case GLFW_KEY_RIGHT_ALT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::RIGHT_ALT, action == GLFW_PRESS);
            break;
        case GLFW_KEY_RIGHT_SUPER:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::RIGHT_SUPER, action == GLFW_PRESS);
            break;
        case GLFW_KEY_MENU:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::MENU, action == GLFW_PRESS);
            break;
        case GLFW_KEY_0:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_0, action == GLFW_PRESS);
            break;
        case GLFW_KEY_1:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_1, action == GLFW_PRESS);
            break;
        case GLFW_KEY_2:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_2, action == GLFW_PRESS);
            break;
        case GLFW_KEY_3:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_3, action == GLFW_PRESS);
            break;
        case GLFW_KEY_4:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_4, action == GLFW_PRESS);
            break;
        case GLFW_KEY_5:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_5, action == GLFW_PRESS);
            break;
        case GLFW_KEY_6:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_6, action == GLFW_PRESS);
            break;
        case GLFW_KEY_7:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_7, action == GLFW_PRESS);
            break;
        case GLFW_KEY_8:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_8, action == GLFW_PRESS);
            break;
        case GLFW_KEY_9:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::NUM_9, action == GLFW_PRESS);
            break;
        case GLFW_KEY_A:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::A, action == GLFW_PRESS);
            break;
        case GLFW_KEY_B:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::B, action == GLFW_PRESS);
            break;
        case GLFW_KEY_C:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::C, action == GLFW_PRESS);
            break;
        case GLFW_KEY_D:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::D, action == GLFW_PRESS);
            break;
        case GLFW_KEY_E:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::E, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F, action == GLFW_PRESS);
            break;
        case GLFW_KEY_G:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::G, action == GLFW_PRESS);
            break;
        case GLFW_KEY_H:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::H, action == GLFW_PRESS);
            break;
        case GLFW_KEY_I:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::I, action == GLFW_PRESS);
            break;
        case GLFW_KEY_J:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::J, action == GLFW_PRESS);
            break;
        case GLFW_KEY_K:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::K, action == GLFW_PRESS);
            break;
        case GLFW_KEY_L:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::L, action == GLFW_PRESS);
            break;
        case GLFW_KEY_M:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::M, action == GLFW_PRESS);
            break;
        case GLFW_KEY_N:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::N, action == GLFW_PRESS);
            break;
        case GLFW_KEY_O:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::O, action == GLFW_PRESS);
            break;
        case GLFW_KEY_P:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::P, action == GLFW_PRESS);
            break;
        case GLFW_KEY_Q:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::Q, action == GLFW_PRESS);
            break;
        case GLFW_KEY_R:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::R, action == GLFW_PRESS);
            break;
        case GLFW_KEY_S:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::S, action == GLFW_PRESS);
            break;
        case GLFW_KEY_T:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::T, action == GLFW_PRESS);
            break;
        case GLFW_KEY_U:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::U, action == GLFW_PRESS);
            break;
        case GLFW_KEY_V:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::V, action == GLFW_PRESS);
            break;
        case GLFW_KEY_W:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::W, action == GLFW_PRESS);
            break;
        case GLFW_KEY_X:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::X, action == GLFW_PRESS);
            break;
        case GLFW_KEY_Y:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::Y, action == GLFW_PRESS);
            break;
        case GLFW_KEY_Z:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::Z, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F1:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F1, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F2:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F2, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F3:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F3, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F4:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F4, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F5:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F5, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F6:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F6, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F7:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F7, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F8:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F8, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F9:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F9, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F10:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F10, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F11:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F11, action == GLFW_PRESS);
            break;
        case GLFW_KEY_F12:
            INPUTS->add_input(input::input_type::INPUT_TYPE_KEYBOARD, input::key::F12, action == GLFW_PRESS);
            break;
        default:
            break;
    }
}

void scroll_callback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] const double offset_x,
    [[maybe_unused]] const double offset_y
) {
    INPUTS->add_input(
        input::input_type::INPUT_TYPE_MOUSE,
        input::key::MOUSE_SCROLL,
        point2Di(static_cast<int>(offset_x), static_cast<int>(offset_y))
    );
}

void cursor_position_callback(
    [[maybe_unused]] GLFWwindow* window,
    const double pos_x,
    const double pos_y
) {
    INPUTS->add_input(
        input::input_type::INPUT_TYPE_MOUSE,
        input::key::MOUSE_MOVE,
        point2Di(static_cast<int>(pos_x), static_cast<int>(pos_y))
    );
}

void mouse_button_callback(
    [[maybe_unused]] GLFWwindow* window,
    const int button,
    const int action,
    [[maybe_unused]] const int mods
) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_MOUSE, input::key::MOUSE_LEFT, action == GLFW_PRESS);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            INPUTS->add_input(input::input_type::INPUT_TYPE_MOUSE, input::key::MOUSE_RIGHT, action == GLFW_PRESS);
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            INPUTS->add_input(input::input_type::INPUT_TYPE_MOUSE, input::key::MOUSE_MIDDLE, action == GLFW_PRESS);
            break;
        default:
            break;
    }
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

class ExamplePass final : public render::RenderPass, public input::InputPass {
public:
    ExamplePass(
        const nvrhi::DeviceHandle& device,
        const std::shared_ptr<render::FontFactory>& font_factory,
        const std::shared_ptr<render::ShaderFactory>& shader_factory,
        const std::shared_ptr<render::TextureFactory>& texture_factory
    )
        : RenderPass(device)
        , _geometry_2d(
            _device,
            font_factory,
            shader_factory,
            texture_factory
        )
        , _blit(device, shader_factory)
        , _image(device)
        , _frame_buffer(device) {

        _command_list = _device->createCommandList();

        _cat_image  = _geometry_2d.add_texture("cat.jpg").value();
        _arial_font = _geometry_2d.add_font("arial.ttf", 20).value();
    }

    void update_input(const bitflag type, const input::InputObserver& input) override {
        if (input.get_state(input::key::MOUSE_LEFT).has(input::button_state::BUTTON_STATE_PRESSED)) {
            if (input.in_rect(_pos, {200 , 200})) {
                _dragging = true;
            }
        }
        else if (_dragging) {
            if (input.get_state(input::key::MOUSE_LEFT).has(input::button_state::BUTTON_STATE_DOWN)) {
                const auto& mouseDelta = input.cursor_delta();
                _pos = {
                    std::clamp<int>(static_cast<int>(_pos.x) + mouseDelta.x, 0, _screen_size.x - 200),
                    std::clamp<int>(static_cast<int>(_pos.y) + mouseDelta.y, 0, _screen_size.y - 200)
                };
            }
            else {
                _dragging = false;
            }
        }
    }

    void update_frame(const render::FrameInterval& interval) override {
        _geometry_2d.d_texture(_pos, {200, 200}, _cat_image);
        _geometry_2d.d_line(_pos, {400, 400}, { 0, 0, 255, 255 }, { 255, 0, 0, 255 });
        _geometry_2d.d_text(_pos, {255, 255, 255}, _arial_font, "TEST FUCK SHIT :D", render::font_flags::Outline);
        _geometry_2d.d_box({100, 100}, {150, 100}, {200, 200}, {100, 200}, {255, 255, 255}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255});
        _geometry_2d.d_box_fill({100 + 200, 100 + 200}, {150 + 200, 100 + 200}, {200 + 200, 200 + 200}, {100 + 200, 200 + 200}, {255, 255, 255}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255});
    }

    void render(nvrhi::IFramebuffer* frame_buffer) override {
        _command_list->open();
        {
            const auto geometry_fb = _frame_buffer[FrameBuffer_Id::Geometry_2d];
            nvrhi::utils::ClearColorAttachment(_command_list, geometry_fb, 0, nvrhi::Color(0));

            _geometry_2d.draw_geometry(_command_list, geometry_fb);
        }
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

        _screen_size = size;
    }

private:
    render::Geometry_2D _geometry_2d;
    render::TextureBlit _blit;
    Image _image;
    FrameBuffer _frame_buffer;

    nvrhi::CommandListHandle _command_list;

    render::geometry::Texture_Id _cat_image;
    render::geometry::Font_Id _arial_font;

    point2Di _screen_size = {};
    point2Df _pos = { 200, 200 };

    bool _dragging = false;
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

    INPUTS  = std::make_unique<input::Input>();
    RENDER  = std::make_unique<render::Render>(LOGGER, settings.value(), render::RenderAPI::Vulkan, true);

    {
        std::shared_ptr<render::FontFactory> FONT_FACTORY       = nullptr;
        std::shared_ptr<render::ShaderFactory> SHADER_FACTORY   = nullptr;
        std::shared_ptr<render::TextureFactory> TEXTURE_FACTORY = nullptr;

        FONT_FACTORY = std::make_shared<render::FontFactory>(
            EXE_PATH / "textures"
        );

        SHADER_FACTORY = std::make_shared<render::ShaderFactory>(
            RENDER->backend()->device_handle(),
            EXE_PATH / "shaders" / shader_type(RENDER->backend()->api())
        );

        TEXTURE_FACTORY = std::make_shared<render::TextureFactory>(
            RENDER->backend()->device_handle(),
            EXE_PATH / "textures"
        );

        const auto triangle_pass    = std::make_unique<render::BasicTriangle>(RENDER->backend()->device_handle(), SHADER_FACTORY);
        const auto example_pass     = std::make_unique<ExamplePass>(RENDER->backend()->device_handle(), FONT_FACTORY, SHADER_FACTORY, TEXTURE_FACTORY);
        const auto gui_pass         = std::make_unique<gui::Gui>(RENDER->backend()->device_handle(), FONT_FACTORY, SHADER_FACTORY, TEXTURE_FACTORY);

        INPUTS->emplace_pass(example_pass.get());
        INPUTS->emplace_pass(gui_pass.get());

        RENDER->emplace_render_pass_back(triangle_pass.get());
        RENDER->emplace_render_pass_back(example_pass.get());
        RENDER->emplace_render_pass_back(gui_pass.get());

        // Main window loop
        while (!glfwWindowShouldClose(window)) {

            // Update input
            glfwPollEvents();

            // Draw
            glfwGetWindowSize(window, &window_size.x, &window_size.y);

            RENDER->update_screen_size(window_size);
            RENDER->present();
        }
    }

    log.v("exited main loop");

    INPUTS.reset();
    RENDER.reset();

    return 0;
}
