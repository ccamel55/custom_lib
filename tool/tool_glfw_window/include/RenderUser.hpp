#pragma once

#include <module_core/type/point/point2D.hpp>

class RenderUser {
public:
    RenderUser();
    ~RenderUser();

    void init();
    void update_screen_size(const lib::point2Di& window_size, bool minimised);
    void on_frame(bool minimised);
};
