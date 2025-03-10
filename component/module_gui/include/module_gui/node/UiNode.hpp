#pragma once

#include <module_core/type/point/point4D.hpp>
#include <module_input/Input.hpp>
#include <module_render/FrameInterval.hpp>
#include <module_render/geometry/Geometry_2D.hpp>

#include <memory>

namespace lib::gui {

class StyleProvider;

extern std::shared_ptr<StyleProvider> STYLER;

//! Base node all UI derives from
class UiNode {
public:
    virtual ~UiNode() = default;

    UiNode(const UiNode&) = delete;
    UiNode(const UiNode&&) = delete;

    UiNode& operator=(const UiNode&) = delete;
    UiNode& operator=(const UiNode&&) = delete;

    //! Update animations
    virtual void OnAnimate(const render::FrameInterval& interval) = 0;

    //! Update render target
    virtual void OnRender(render::Geometry_2D& geometry_2d) = 0;

    //! Update inputs
    virtual void OnUpdate(bitflag input_type, const input::InputObserver& input) = 0;

private:
    lib::point4Di m_rect = {};

};

}