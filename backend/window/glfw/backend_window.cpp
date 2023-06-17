#include <backend/window/glfw/backend_window.hpp>

using namespace lib::backend;

namespace {

    // passing c callbacks into c++ ew
    backend_input_base* input_handler_ptr = nullptr;

    void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        input_handler_ptr->update_input_state(key, action);
    }
}

backend_window::~backend_window() {

    input_handler_ptr = nullptr;

    if (_render_handler) {
        _render_handler->destroy_instance();
    }
}

void backend_window::create_window(const std::string& window_name, int pos_x, int pos_y, int width, int height, WINDOW_FLAGS_E flags) {

    // initialize base class
    backend_window_base::create_window(window_name, pos_x, pos_y, width, height, flags);

    // try init glfw
    if (!glfwInit()) {
        throw std::invalid_argument("backend_window: failed to initialize glfw");
    }

    // disable msaa
    glfwWindowHint(GLFW_SAMPLES, 0);

    // no backend
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // remove border and title bar
    glfwWindowHint(GLFW_DECORATED, !(flags & WINDOW_FLAG_NO_BORDER));

    // create a new window
    _glfw_window_ptr = glfwCreateWindow( _window_size._x, _window_size._y, _window_name.data(), nullptr, nullptr);

    if(!_glfw_window_ptr) {
        glfwTerminate();
        throw std::invalid_argument("backend_window: could not create window");
    }

    // move to correct location
    glfwSetWindowPos(_glfw_window_ptr, _window_position._x, _window_position._y);

    // focus window
    glfwMakeContextCurrent(_glfw_window_ptr); // Initialize GLEW

    // make sure we can capture escape key
    glfwSetInputMode(_glfw_window_ptr, GLFW_STICKY_KEYS, GL_TRUE);

    if (_render_handler) {
        _render_handler->init_instance();
    }

    if (_input_handler) {

        input_handler_ptr = _input_handler.get();
        glfwSetKeyCallback(_glfw_window_ptr, glfw_key_callback);
    }
};

void backend_window::close_window() {
    glfwSetWindowShouldClose(_glfw_window_ptr, true);
};

void backend_window::focus_window() {
    glfwMakeContextCurrent(_glfw_window_ptr);
};

void backend_window::window_loop() {

    // run main render thread from current thread
    while (glfwGetKey(_glfw_window_ptr, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(_glfw_window_ptr) == 0 ) {

        // draw stuff here
        if (_render_callback) {
            _render_callback();
        }

        // Poll for inputs
        glfwPollEvents();
    }

    // stop glfw
    glfwTerminate();
}