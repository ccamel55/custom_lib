#pragma once

#include <mutex>
#include <thread>

#include <GLFW/glfw3.h>
#include <backend/window/backend_window_base.hpp>

namespace lib::backend {

    class backend_window : public backend_window_base {
    public:
        backend_window(std::string_view window_name, int pos_x, int pos_y, int width, int height, WINDOW_FLAGS_E flags = WINDOW_FLAGS_E::WINDOW_FLAG_NONE);

        void create_window() override;
        void close_window() override;
        void focus_window() override;
    private:
        GLFWwindow* _glfw_window_ptr{};
    };
}