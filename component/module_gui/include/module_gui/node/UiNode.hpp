#pragma once

#include <module_input/Input.hpp>
#include <module_render/FrameInterval.hpp>
#include <module_render/geometry/Geometry_2D.hpp>

#include <memory>

namespace lib::gui {

class StyleProvider;

extern std::unique_ptr<StyleProvider> STYLER;

struct node_properties_t {
    lib::point4Di area;
};

//! Base node all UI derives from
class UiNode {
public:
    UiNode() = default;
    virtual ~UiNode() = default;

    UiNode(const UiNode&) = delete;
    UiNode(const UiNode&&) = delete;

    UiNode& operator=(const UiNode&) = delete;
    UiNode& operator=(const UiNode&&) = delete;

    //! Update animations
    virtual void OnAnimate(const render::FrameInterval& interval) = 0;

    //! Update inputs
    virtual void OnInput(bitflag input_type, const input::InputObserver& input) = 0;

    //! Update window size/compute size
    virtual void OnRefresh() = 0;

    //! Update render target
    virtual void OnRender(render::Geometry_2D& geometry_2d) = 0;

    //! Get node properties.
    [[nodiscard]] const node_properties_t& GetNodeProperties() const;

protected:
    //! Get node properties.
    [[nodiscard]] node_properties_t& GetNodeProperties();

protected:
    node_properties_t m_nodeProperties = {};

};

}