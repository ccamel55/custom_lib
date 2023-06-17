#include <backend/render/opengl3/backend_render.hpp>

using namespace lib::backend;

void backend_render::init_instance() {

}

void backend_render::destroy_instance() {

}

void backend_render::bind_context(void *context) {

}

void backend_render::remove_context() {

}

void backend_render::reset() {

}

void backend_render::render_start() {

}

void backend_render::render_finish() {

}

void backend_render::addFont(
        lib::common::hash_t font_hash,
        const std::string &font_name, size_t height,
        size_t weight) {

}

void backend_render::draw_string(
        lib::common::hash_t font_hash,
        const lib::common::point_int &pos,
        const lib::common::color &color,
        const std::string &string,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_rect(
        const lib::common::point_int &pos,
        const lib::common::point_int &size,
        const lib::common::color &color,
        RENDER_FLAGS_E flags) {

}

void
backend_render::draw_rect_gradient(
        const lib::common::point_int &pos,
        const lib::common::point_int &size,
        const lib::common::color &color1,
        const lib::common::color &color2,
        RENDER_FLAGS_E flags) {

}

void
backend_render::draw_rect_filled(
        const lib::common::point_int &pos,
        const lib::common::point_int &size,
        const lib::common::color &color,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_rect_filled_gradient(
        const lib::common::point_int &pos,
        const lib::common::point_int &size,
        const lib::common::color &color1,
        const lib::common::color &color2,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_circle(
        const lib::common::point_int &pos,
        float radius,
        const lib::common::color &color,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_circle_filled(
        const lib::common::point_int &pos, float radius,
        const lib::common::color &color,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_circle_filled_gradient(
        const lib::common::point_int &pos,
        float radius,
        const lib::common::color &color1,
        const lib::common::color &color2,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_triangle(
        const lib::common::point_int &pos1,
        const lib::common::point_int &pos2,
        const lib::common::point_int &pos3,
        const lib::common::color &color,
        RENDER_FLAGS_E flags) {

}

void backend_render::draw_triangle_filled(
        const lib::common::point_int &pos1,
        const lib::common::point_int &pos2,
        const lib::common::point_int &pos3,
        const lib::common::color &color,
        RENDER_FLAGS_E flags) {

}
