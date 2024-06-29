#pragma once

#include <module_render/RenderConsumer.hpp>

namespace lib::render
{
// Alias correct type
class Render;
using RenderConsumer = RenderConsumer_Base<Render>;

//! Render handler
class Render : public RenderConsumer {
    friend class RenderConsumer_Base;

public:
    Render() = default;
    ~Render() = default;

private:
    void internal_some_func() {

    }
};}