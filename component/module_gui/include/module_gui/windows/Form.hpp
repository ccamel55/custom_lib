#pragma once

#include <module_gui/node/WindowNode.hpp>

namespace lib::gui {

class Form : public WindowNode {

    struct defaults {
        static constexpr int WIDTH  = 640;
        static constexpr int HEIGHT = 480;
    };

public:
    explicit Form(
        std::string title,
        const lib::point2Di& size   = { defaults::WIDTH, defaults::HEIGHT },
        const lib::point2Di& pos    = { 50, 50 }
    );

    //
    // Override
    //

    void OnAnimate(const render::FrameInterval& interval) override;
    void OnInput(bitflag input_type, const input::InputObserver& input) override;
    void OnRefresh() override;
    void OnRender(render::Geometry_2D& geometry_2d) override;

    //
    // Implementation
    //

    //! Register content for form
    //! \param content content to be displayed
    void SetContent(std::unique_ptr<UiNode>&& content);

private:
    std::string m_title;

    bool m_resizing;
    bool m_dragging;

    std::unique_ptr<UiNode> m_content;

};

}