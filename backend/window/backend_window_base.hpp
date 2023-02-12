#pragma once

#include <singleton.hpp>
#include <common/types/point/point_int.hpp>
#include <common/types/bitflag.hpp>

#include <string>
#include <exception>
#include <utility>

#include <GLFW/glfw3.h>

namespace lib::backend {

    enum WINDOW_FLAGS_E : common::bitflag_t {
        WINDOW_FLAG_NONE = 0 << 0,
        WINDOW_FLAG_NO_BORDER = 1 << 0,
    };

    //! base class for os window creation
	class backend_window_base : public shared_singleton<backend_window_base> {
    public:
        //! register a callback function \param render_callback that is called every render frame
        void register_render_callback(std::function<void()> render_callback) {
            _render_callback = std::move(render_callback);
        }

        //! return the size of the current window
        [[nodiscard]] const common::point_int& get_size() const {
            return _window_size;
        }

        //! create a new os window
        virtual void create_window(const std::string& window_name, int pos_x, int pos_y, int width, int height, WINDOW_FLAGS_E flags) {

            _window_name = window_name;

            _window_position = {pos_x, pos_y};
            _window_size = {width, height};

            _flags = WINDOW_FLAGS_E::WINDOW_FLAG_NONE;
        }

        //! close the os window
        virtual void close_window() = 0;

        //! bring the os window to focus
        virtual void focus_window() = 0;

        //! run the loop that handles inputs from the window
        virtual void window_loop() = 0;
	protected:
        std::function<void()> _render_callback = nullptr;
		std::string _window_name;

		common::point_int _window_position = {100, 100};
		common::point_int _window_size = {720, 480};

        WINDOW_FLAGS_E _flags = WINDOW_FLAGS_E::WINDOW_FLAG_NONE;
	};
}