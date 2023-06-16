#pragma once

#include <backend/render/backend_render_base.hpp>

namespace lib::backend {

    class backend_render : public backend_render_base {
    public:
        void init_instance() override;
        void destroy_instance() override;
        void bind_context(void *context) override;
        void remove_context() override;
        void reset() override;
        void render_start() override;
        void render_finish() override;
        void addFont(common::hash_t font_hash, const std::string &font_name, size_t height, size_t weight) override;
        void draw_string(common::hash_t font_hash, const common::point_int &pos, const common::color &color,const std::string &string, RENDER_FLAGS_E flags) override;
        void draw_rect(const common::point_int &pos, const common::point_int &size, const common::color &color,RENDER_FLAGS_E flags) override;
        void draw_rect_gradient(const common::point_int &pos, const common::point_int &size, const common::color &color1,const common::color &color2, RENDER_FLAGS_E flags) override;
        void draw_rect_filled(const common::point_int &pos, const common::point_int &size, const common::color &color,RENDER_FLAGS_E flags) override;
        void draw_rect_filled_gradient(const common::point_int &pos, const common::point_int &size,const common::color &color1, const common::color &color2,RENDER_FLAGS_E flags) override;
        void draw_circle(const common::point_int &pos, float radius, const common::color &color,RENDER_FLAGS_E flags) override;
        void draw_circle_filled(const common::point_int &pos, float radius, const common::color &color,RENDER_FLAGS_E flags) override;
        void draw_circle_filled_gradient(const common::point_int &pos, float radius, const common::color &color1,const common::color &color2, RENDER_FLAGS_E flags) override;
        void draw_triangle(const common::point_int &pos1, const common::point_int &pos2, const common::point_int &pos3,const common::color &color, RENDER_FLAGS_E flags) override;
        void draw_triangle_filled(const common::point_int &pos1, const common::point_int &pos2, const common::point_int &pos3, const common::color &color, RENDER_FLAGS_E flags) override;
    private:

    };
}