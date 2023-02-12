#pragma once

#include <singleton.hpp>

#include <common/cryptography/fn1v.hpp>
#include <common/types/point/point_int.hpp>
#include <common/types/point/point_float.hpp>
#include <common/types/bitflag.hpp>
#include <common/types/color.hpp>

#include <array>
#include <unordered_map>

namespace lib::backend {

    constexpr uint8_t CIRCLE_CACHE_SEGMENTS = 64;

    enum RENDER_FLAGS_E : common::bitflag_t {
        RENDER_FLAG_NONE = 0 << 0,
        RENDER_FLAG_OUTLINE = 1 << 0,
        RENDER_FLAG_CENTER_X = 1 << 1,
        RENDER_FLAG_CENTER_Y = 1 << 2,
        RENDER_FLAG_GRADIENT_HOR = 1 << 3,
        RENDER_FLAG_GRADIENT_VER= 1 << 4,
    };

    struct circle_cache_t {
        float sin = 0.f;
        float cos = 0.f;
    };

    //! base class for renderer implementation, renderer can be instantiated through the singleton or created as a
    //! shared pointer to be used on different windows
    class backend_render_base : public shared_singleton<backend_render_base> {
    public:
        backend_render_base() {

            // cache circle segments
            for (size_t i = 0; i <= CIRCLE_CACHE_SEGMENTS; i++) {
                _circle_cache.at(i).sin = std::sin(6.283185f * (i / static_cast<float>(CIRCLE_CACHE_SEGMENTS)));
                _circle_cache.at(i).cos = std::cos(6.283185f * (i / static_cast<float>(CIRCLE_CACHE_SEGMENTS)));
            }
        }

        //! returns the cos/sin value of a particular circle segment
        const circle_cache_t& get_circle_cache(uint8_t segment) const  {
            return _circle_cache.at(segment);
        }

        //! returns the time between frames
        const float get_frame_time() const {
            return _frame_time;
        }

        //! returns the size of the window the renderer is running in
        const common::point_int& get_window_size() const {
            return _window_size;
        }
    public:
        //! create an instance of the rendering context
        virtual void init_instance() = 0;

        //! destroys current render instance
        virtual void destroy_instance() = 0;

        //! bind the render context to the renderer, used when not calling \a init_instance
        virtual void bind_context(void* context) = 0;

        //! removes the render context, used when not calling \a destroy_context
        virtual void remove_context() = 0;

        //! resets the render context
        virtual void reset() = 0;

        //! called before starting drawing in draw loop
        virtual void render_start() = 0;

        //! called at the end of drawing in draw loop
        virtual void render_finish() = 0;

        //! create a new font with the given param, \param font_hash is the name of the font hashed to fn1v
        virtual void addFont(common::hash_t font_hash, const std::string& font_name, size_t height, size_t weight) = 0;

        //! draw a string
        virtual void draw_string(common::hash_t font_hash, const common::point_int& pos, const common::color& color, const std::string& string, RENDER_FLAGS_E flags) = 0;

        //! draw a rectangle
        virtual void draw_rect(const common::point_int& pos, const common::point_int& size, const common::color& color, RENDER_FLAGS_E flags) = 0;

        //! draw a gradient rectangle
        virtual void draw_rect_gradient(const common::point_int& pos, const common::point_int& size, const common::color& color1, const common::color& color2, RENDER_FLAGS_E flags) = 0;

        //! draw a filled rectangle
        virtual void draw_rect_filled(const common::point_int& pos, const common::point_int& size, const common::color& color, RENDER_FLAGS_E flags) = 0;

        //! draw a filled gradient rectangle
        virtual void draw_rect_filled_gradient(const common::point_int& pos, const common::point_int& size, const common::color& color1, const common::color& color2, RENDER_FLAGS_E flags) = 0;

        //! draw a circle
        virtual void draw_circle(const common::point_int& pos, float radius, const common::color& color, RENDER_FLAGS_E flags) = 0;

        //! draw a filled circle
        virtual void draw_circle_filled(const common::point_int& pos, float radius, const common::color& color, RENDER_FLAGS_E flags) = 0;

        //! draw a filled gradient circle
        virtual void draw_circle_filled_gradient(const common::point_int& pos, float radius, const common::color& color1, const common::color& color2, RENDER_FLAGS_E flags) = 0;

        //! draw a triangle
        virtual void draw_triangle(const common::point_int& pos1, const common::point_int& pos2, const common::point_int& pos3, const common::color& color, RENDER_FLAGS_E flags) = 0;

        //! draw a filled triangle
        virtual void draw_triangle_filled(const common::point_int& pos1, const common::point_int& pos2, const common::point_int& pos3, const common::color& color, RENDER_FLAGS_E flags) = 0;
    private:
        float _frame_time = 0.f;
        common::point_int _window_size = {};

        std::array<circle_cache_t, CIRCLE_CACHE_SEGMENTS> _circle_cache = {};
    };
}