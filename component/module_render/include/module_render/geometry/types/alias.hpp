#pragma once

#include <module_render/types/keys.hpp>

#include <module_render/util/FontFactory.hpp>
#include <module_render/util/Pipeline.hpp>

#include <list>

namespace lib::render::geometry {

enum class Pipeline_Id: uint32_t {
    Geometry_Texture,
    Geometry_Texture_Sdf,
    Geometry_Texture_Sdf_Outline,

    // Must always be last
    Num_Pipeline_Id
};

using TextureList   = std::list<nvrhi::TextureHandle>;
using Texture_Id    = TextureList::iterator;

struct font_handle_t {
    Texture_Id texture;
    std::array<font_character_t, CHAR_END - CHAR_START> characters = {};
};

using FontList      = std::list<font_handle_t>;
using Font_Id       = FontList::iterator;

using Pipeline     = Pipeline<Pipeline_Id, static_cast<size_t>(Pipeline_Id::Num_Pipeline_Id)>;
using BindingSet   = std::unordered_map<binding_set_desc_key, nvrhi::BindingSetHandle, binding_set_desc_key::hash>;

}