#pragma once

#include <module_gui/node/ContainerNode.hpp>

namespace lib::gui {

struct flexbox_properties_t {

};

class Flexbox : public ContainerNode {

    struct defaults {
        static constexpr int WIDTH  = 640;
        static constexpr int HEIGHT = 480;

        static constexpr int DRAG_HEIGHT = 20;
        static constexpr int RESIZE_SIZE = 10;
    };

public:

};

}