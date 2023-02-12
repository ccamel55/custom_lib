#pragma once

#include <memory>
#include <string>
#include <exception>

#include <GLFW/glfw3.h>
#include <common/types/point/point_int.hpp>
#include <common/types/bitflag.hpp>
#include <utility>

namespace lib::backend {

    enum WINDOW_FLAGS_E : common::bitflag_t {
        WINDOW_FLAG_NONE = 0 << 0,
        WINDOW_FLAG_NO_BORDER = 1 << 0,
    };

	class backend_window_base {
    public:
        static std::shared_ptr<backend_window_base> get(std::shared_ptr<backend_window_base> impl = nullptr) {

            static std::shared_ptr<backend_window_base> _impl = nullptr;

            // set implementation
            if (impl != nullptr) {
                _impl = std::move(impl);
            }

            return _impl;
        }

        backend_window_base(std::string_view window_name, int pos_x, int pos_y, int width, int height, WINDOW_FLAGS_E flags = WINDOW_FLAGS_E::WINDOW_FLAG_NONE)
            : _window_name(window_name), _window_position(pos_x, pos_y), _window_size(width, height), _flags(flags) {
        }

        [[maybe_unused]] virtual void create_window() = 0;
        [[maybe_unused]] virtual void close_window() = 0;
        [[maybe_unused]] virtual void focus_window() = 0;
	protected:
		std::string_view _window_name{};

		common::point_int _window_position{};
		common::point_int _window_size{};

        WINDOW_FLAGS_E _flags{};
	};
}