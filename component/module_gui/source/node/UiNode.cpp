#include <module_gui/node/UiNode.hpp>

using namespace lib::gui;

void UiNode::OnRefresh(const StyleProvider* styler) {
    m_styler = styler;
}

const node_properties_t& UiNode::GetNodeProperties() const {
    return m_nodeProperties;
}

node_properties_t& UiNode::GetNodeProperties() {
    return m_nodeProperties;
}