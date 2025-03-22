#include <module_gui/node/UiNode.hpp>

using namespace lib::gui;

const node_properties_t& UiNode::GetNodeProperties() const {
    return m_nodeProperties;
}

node_properties_t& UiNode::GetNodeProperties() {
    return m_nodeProperties;
}