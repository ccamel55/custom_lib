#pragma once

#include <module_core/NoCopy.hpp>

#include <module_render/geometry/types/alias.hpp>
#include <module_render/geometry/types/vertex.hpp>

namespace lib::render {

enum font_flags : bitflag_t {
    None = 0,
    Align_L = 0 << 0,
    Align_R = 1 << 0,
    Centre_X = 1 << 1,
    Centre_Y = 1 << 2,
    Centre_XY = Centre_X | Centre_Y,
    Outline = 1 << 3,
};

//! Interface for 2D geometry rendering
template<typename Base>
class Geometry_2D_Base : public NoCopy {
public:
    //! Draw a texture in full
    //! \param pos position in world space
    //! \param size size in world unit length
    //! \param texture texture to draw
    //! \param alpha opacity of texture
    void d_texture(
        const point2Df& pos,
        const point2Df& size,
        const geometry::Texture_Id& texture,
        uint8_t alpha = 255
    ) {
        return static_cast<Base*>(this)->d_texture(
            pos,
            size,
            texture,
            alpha
         );
    }

    //! Draw line
    //! \param pos_1 starting point in world space
    //! \param pos_2 end point in world space
    //! \param color_1 line gradient color start
    //! \param color_2 line gradient color end
    void d_line(
        const point2Df& pos_1,
        const point2Df& pos_2,
        const color& color_1,
        const color& color_2
    ) {
        return static_cast<Base*>(this)->d_line(
            pos_1,
            pos_2,
            color_1,
            color_2
        );
    }

    //! Draw triangle
    //! \param vert_1 vertex 1
    //! \param vert_2 vertex 2
    //! \param vert_3 vertex 3
    //! \param color vertex color
    void d_triangle(
        const point2Df& vert_1,
        const point2Df& vert_2,
        const point2Df& vert_3,
        const color& color
    ) {
        d_triangle(vert_1, vert_2, vert_3, color, color, color);
    }

    //! Draw triangle
    //! \param vert_1 vertex 1
    //! \param vert_2 vertex 2
    //! \param vert_3 vertex 3
    //! \param color_1 vertex 1 color
    //! \param color_2 vertex 2 color
    //! \param color_3 vertex 3 color
    void d_triangle(
        const point2Df& vert_1,
        const point2Df& vert_2,
        const point2Df& vert_3,
        const color& color_1,
        const color& color_2,
        const color& color_3
    ) {
        d_line(vert_1, vert_2, color_1, color_2);
        d_line(vert_2, vert_3, color_2, color_3);
        d_line(vert_3, vert_1, color_3, color_1);
    }

    //! Draw filled triangle
    //! \param vert_1 vertex 1
    //! \param vert_2 vertex 2
    //! \param vert_3 vertex 3
    //! \param color vertex color
    void d_triangle_filled(
        const point2Df& vert_1,
        const point2Df& vert_2,
        const point2Df& vert_3,
        const color& color
    ) {
        d_triangle_filled(vert_1, vert_2, vert_3, color, color, color);
    }

    //! Draw filled triangle
    //! \param vert_1 vertex 1
    //! \param vert_2 vertex 2
    //! \param vert_3 vertex 3
    //! \param color_1 vertex 1 color
    //! \param color_2 vertex 2 color
    //! \param color_3 vertex 3 color
    void d_triangle_filled(
        const point2Df& vert_1,
        const point2Df& vert_2,
        const point2Df& vert_3,
        const color& color_1,
        const color& color_2,
        const color& color_3
    ) {
        return static_cast<Base*>(this)->d_triangle_filled(
            vert_1,
            vert_2,
            vert_3,
            color_1,
            color_2,
            color_3
        );
    }

    //! Draw box
    //! \param area area to draw box
    //! \param color color of box
    void d_box(
        const point4Df& area,
        const color& color
    ) {
        d_box(
            area,
            color,
            color,
            color,
            color
        );
    }

    //! Draw box
    //! \param area area to draw box
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box(
        const point4Df& area,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    ) {
        const lib::point2Df pos_tl = point2Df(area.x, area.y);
        const lib::point2Df pos_tr = point2Df(area.x + area.z, area.y);
        const lib::point2Df pos_br = point2Df(area.x + area.z, area.y + area.w);
        const lib::point2Df pos_bl = point2Df(area.x, area.y + area.w);

        d_box(
            pos_tl,
            pos_tr,
            pos_br,
            pos_bl,
            color_tl,
            color_tr,
            color_br,
            color_bl
        );
    }

    //! Draw box
    //! \param pos position of top left corner
    //! \param size size of box
    //! \param color color of box
    void d_box(
        const point2Df& pos,
        const point2Df& size,
        const color& color
    ) {
        d_box(
            pos,
            size,
            color,
            color,
            color,
            color
        );
    }

    //! Draw box
    //! \param pos position of top left corner
    //! \param size size of box
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box(
        const point2Df& pos,
        const point2Df& size,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    ) {
        const lib::point2Df pos_tl = pos;
        const lib::point2Df pos_tr = pos + point2Df(size.x, 0.f);
        const lib::point2Df pos_br = pos + point2Df(size.x, size.y);
        const lib::point2Df pos_bl = pos + point2Df(0.f, size.y);

        d_box(
            pos_tl,
            pos_tr,
            pos_br,
            pos_bl,
            color_tl,
            color_tr,
            color_br,
            color_bl
        );
    }

    //! Draw box
    //! \param pos_tl position of top left corner
    //! \param pos_tr position of top right corner
    //! \param pos_br position of bottom right corner
    //! \param pos_bl position of bottom left corner
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box(
        const point2Df& pos_tl,
        const point2Df& pos_tr,
        const point2Df& pos_br,
        const point2Df& pos_bl,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    ) {
        // Lazy man's way, just call `d_line` 4 times lol
        d_line(pos_tl, pos_tr, color_tl, color_tr);
        d_line(pos_tr, pos_br, color_tr, color_br);
        d_line(pos_br, pos_bl, color_br, color_bl);
        d_line(pos_bl, pos_tl, color_bl, color_tl);
    }

    //! Draw filled box
    //! \param area area to draw box
    //! \param color color of box
    void d_box_fill(
        const point4Df& area,
        const color& color
    ) {
        d_box_fill(
            area,
            color,
            color,
            color,
            color
        );
    }

    //! Draw filled box
    //! \param area area to draw box
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box_fill(
        const point4Df& area,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    ) {
        const lib::point2Df pos_tl = point2Df(area.x, area.y);
        const lib::point2Df pos_tr = point2Df(area.x + area.z, area.y);
        const lib::point2Df pos_br = point2Df(area.x + area.z, area.y + area.w);
        const lib::point2Df pos_bl = point2Df(area.x, area.y + area.w);

        d_box_fill(
            pos_tl,
            pos_tr,
            pos_br,
            pos_bl,
            color_tl,
            color_tr,
            color_br,
            color_bl
        );
    }

    //! Draw filled box
    //! \param pos position of top left corner
    //! \param size size of box
    //! \param color color of box
    void d_box_fill(
        const point2Df& pos,
        const point2Df& size,
        const color& color
    ) {
        d_box_fill(
            pos,
            size,
            color,
            color,
            color,
            color
        );
    }

    //! Draw filled box
    //! \param pos position of top left corner
    //! \param size size of box
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box_fill(
        const point2Df& pos,
        const point2Df& size,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    ) {
        const lib::point2Df pos_tl = pos;
        const lib::point2Df pos_tr = pos + point2Df(size.x, 0.f);
        const lib::point2Df pos_br = pos + point2Df(size.x, size.y);
        const lib::point2Df pos_bl = pos + point2Df(0.f, size.y);

        d_box_fill(
            pos_tl,
            pos_tr,
            pos_br,
            pos_bl,
            color_tl,
            color_tr,
            color_br,
            color_bl
        );
    }

    //! Draw filled box
    //! \param pos_tl position of top left corner
    //! \param pos_tr position of top right corner
    //! \param pos_br position of bottom right corner
    //! \param pos_bl position of bottom left corner
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box_fill(
        const point2Df& pos_tl,
        const point2Df& pos_tr,
        const point2Df& pos_br,
        const point2Df& pos_bl,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    ) {
        return static_cast<Base*>(this)->d_box_fill(
            pos_tl,
            pos_tr,
            pos_br,
            pos_bl,
            color_tl,
            color_tr,
            color_br,
            color_bl
        );
    }

    //! Draw text
    //! \param pos position in world space
    //! \param color color of font
    //! \param id font id
    //! \param text string to draw
    //! \param flags draw flags
    void d_text(
        const point2Df& pos,
        const color& color,
        const geometry::Font_Id& id,
        const std::string& text,
        bitflag flags = font_flags::None
    ) {
        return static_cast<Base*>(this)->d_text(
            pos,
            color,
            id,
            text,
            flags
        );
    }
};

}