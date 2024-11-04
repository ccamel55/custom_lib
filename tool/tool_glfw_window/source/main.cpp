#include <iostream>

#include <RenderUser.hpp>

#include <GLFW/glfw3.h>

#include <module_core/type/point/point2D.hpp>
#include <module_logger/Logger.hpp>
#include <module_logger/ScopedLog.hpp>
#include <module_logger/handler/Std_LogHandler.hpp>

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
        return 1;
    }

    log.v("starting main loop");

    bool minimised = false;

    USER = std::make_unique<RenderUser>();
    USER->init();

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

    // Cleanup resources
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}