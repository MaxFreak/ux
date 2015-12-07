//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_GP_WRAP_H
#define UX_GP_WRAP_H

#include "geo_rectangle.h"
#include "ux_types_and_defs.h"

enum image_id {DUMMY_IMAGE};

ux_uint UINT_FROM_RGB(const ux_ubyte & R, const ux_ubyte & G, const ux_ubyte & B);
ux_uint UINT_FROM_ARGB(const ux_ubyte & A, const ux_ubyte & R, const ux_ubyte & G, const ux_ubyte & B);
ux_ubyte GET_A_VAL32(const ux_uint & argb);
ux_ubyte GET_R_VAL32(const ux_uint & argb);
ux_ubyte GET_G_VAL32(const ux_uint & argb);
ux_ubyte GET_B_VAL32(const ux_uint & argb);

class gp_wrap
{
public:

    enum refresh_type
    {
        refresh_single,         ///< Only one refresh in this frame. So it is the first and the last call.
        refresh_first_frame,    ///< The first refresh call in this frame.
        refresh_intermediate,   ///< Any refresh between the first and the last call.
        refresh_last_frame,     ///< The last refresh call in this frame.
        refresh_forced          ///< refresh was forced. A refresh of this type is used in addition to one of the others, see force_refresh().
    };

    gp_wrap();
    virtual ~gp_wrap();

    virtual ux_bool init();
    virtual void de_init();

    inline void force_refresh() { m_force_refresh = true; }

    /** Allocates storage for a virtual screen in which the graphics operations will draw.
        @param width Width of virtual screen.
        @param height Height of virtual screen. */
    virtual void create_screen(const ux_uint &width, const ux_uint &height) {}

    /** Returns the screen width in pixels, may vary if draw_to_bitmap is active.
        It will then return the active bitmap plane's width instead.
        @see get_physical_screen_width()
        @return ux_uint The screen width */
    virtual ux_uint get_virtual_screen_width() = 0;

    /** Returns the screen height in pixels, may vary if draw_to_bitmap is active.
        It will then return the active bitmap plane's height instead.
        @see get_physical_screen_height()
        @return ux_uint The screen height */
    virtual ux_uint get_virtual_screen_height() = 0;

    /** Sets the new physical screen size.
        @param width New screen width.
        @param height New screen height.  */
    virtual void set_screen_size(const ux_uint &width, const ux_uint &height);

    /** Returns the screen width.
        Will always return the physical screen width with which the gp_wrap was originally initialized.
        Regardless of whether the virtual screen size has been modified e.g. while drawing into a CGUIBitmapPlane.
        @return Physical screen width
    */
    virtual ux_uint get_physical_screen_width() = 0;

    /** Returns the screen height.
        Will always return the physical screen height with which the gp_wrap was originally initialized.
        Regardless of whether the virtual screen size has been modified e.g. while drawing into a CGUIBitmapPlane.
        @return Physical screen height
    */
    virtual ux_uint get_physical_screen_height() = 0;

    /** Returns the vertical pixel size in pixel per inch.
        @returns vertical PPI
    */
    virtual ux_uint get_display_vertical_PPI() const {return (72);}          // 72 as default

    /** Returns the horizontal pixel size in pixel per inch.
        @returns horizontal PPI
    */
    virtual ux_uint get_display_horizontal_PPI() const {return (72);}        // 72 as default

    // ---------------------------------------------------------------
    // Drawing functions
    // ---------------------------------------------------------------

    /** Sets the foreground color via red, green, blue and alpha values
        @param red Red intensity within a range of 0 - 255
        @param green Green intensity within a range of 0 - 255
        @param blue Blue intensity within a range of 0 - 255
        @param alpha Alpha intensity within a range of 0 (transparent) - 255 (opaque)
    */
    void set_foreground_color(const ux_ubyte &red, const ux_ubyte &green, const ux_ubyte &blue,
                              const ux_ubyte &alpha = 255)
    {
        set_foreground_color_impl(red, green, blue, apply_global_alpha(alpha));
    }

    /** Sets the foreground color via a 32 Bit value in the Format 0xAARRGGBB
        Example Values:
            0xFFFF0000  = Red (fully opaque)
            0x77FFFFFF  = White (semi transparent)
        @param color 32Bit Color value in the Format 0xAARRGGBB
    */
    void set_foreground_color(const ux_uint &color)
    {
        // The value of alpha is multiplied by the scaling factor (in the event of widgets with text, the text's alpha has to be scaled)
        set_foreground_color_impl(GET_R_VAL32(color), GET_G_VAL32(color), GET_B_VAL32(color),
                                  apply_global_alpha(GET_A_VAL32(color)));
    }

    /** Updates the global alpha via an unsigned Byte
        The global alpha value will affect all drawing operations.
        Subsequent calls to update_global_alpha will blend the new alpha value with
        the formerly set one.
        @see set_global_alpha
        @param alpha unsigned Byte
    */
    void update_global_alpha(const ux_ubyte &alpha)
    {
        m_alpha = apply_global_alpha(alpha);
        m_global_alpha_factor = m_alpha / 255.0f;
    }


    /** Sets the global alpha via an unsigned Byte.
        In contrast to UpdateGlobalAlpha, this will NOT blend subsequent alphas
        @see update_global_alpha
        @param alpha unsigned Byte
    */
    void set_global_alpha(const ux_ubyte &alpha)
    {
        m_alpha = alpha;
        m_global_alpha_factor = m_alpha / 255.0f;
    }

    /** Sets the line width in pixels
        @param width width of lines
        @return The previous line width.
    */
    virtual ux_value set_line_width(const ux_value &width)
    {
        ux_value old_width = m_line_width;
        m_line_width = width;
        return old_width;
    }

    /** Draws a line from (abs_x1, abs_y1) to (abs_x2, abs_y2)
        @param abs_x1 X-coordinate of starting point
        @param abs_y1 Y-Coordinate of starting point
        @param abs_x2 X-coordinate of ending point
        @param abs_y2 Y-Coordinate of ending point
    */
    virtual void line(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2) = 0;

    /** Draws a line defined by a CGUIRect
        @param abs_rect CGUIRect containing the line endpoints
        @see CGfxWrap::line(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2)
    */
    inline void line(const geo::float_rect &abs_rect)
    {
        line(abs_rect.get_x1(), abs_rect.get_y1(), abs_rect.get_y2(), abs_rect.get_y2());
    }

    /** Draws a rectangle with the top left edge at (abs_x1, abs_y1) and
            bottom right edge at (abs_x2, abs_y2).
        The bottom edge and the right edge is not included in the drawn rect.
        e.g. rect(0, 0, 10, 10 ); the rect is drawn from (0,0) to (9,9)
        (both included).
        @see CGUIObject::GetAbsRect()
        @param abs_x1 X-coordinate of top left corner
        @param abs_y1 Y-Coordinate of top left corner
        @param abs_x2 X-coordinate of bottom right corner
        @param abs_y2 Y-Coordinate of bottom right corner
    */
    virtual void rect(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2) = 0;

    /** Draws a rectangle defined by a CGUIRect
        @param abs_rect CGUIRect containing the rectangle corners' endpoints
        @see CGfxWrap::rect(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2)
    */
    inline void rect(const geo::float_rect &abs_rect)
    {
        rect(abs_rect.get_x1(), abs_rect.get_y1(), abs_rect.get_y2(), abs_rect.get_y2());
    }

    /** Draws a filled rectangle with the top left edge at (abs_x1, abs_y1)
            and bottom right edge at (abs_x2, abs_y2)
        The bottom edge and the right edge is not included in the drawn rect.
        e.g. filled_rect(0, 0, 10, 10 ); the rect is drawn from (0,0) to (9,9)
        (both included).
        @see CGUIObject::GetAbsRect()
        @param abs_x1 X-coordinate of top left corner
        @param abs_y1 Y-Coordinate of top left corner
        @param abs_x2 X-coordinate of bottom right corner
        @param abs_y2 Y-Coordinate of bottom right corner
    */
    virtual void filled_rect(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2,
                             const ux_value &abs_y2) = 0;

    /** Draws a filled rectangle defined by a CGUIRect
        @param abs_rect CGUIRect containing the rectangle corners' endpoints
        @see CGfxWrap::filled_rect(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2)
    */
    inline void filled_rect(const geo::float_rect &abs_rect)
    {
        filled_rect(abs_rect.get_x1(), abs_rect.get_y1(), abs_rect.get_y2(), abs_rect.get_y2());
    }

    /** Draws an ellipse which is complete inside the rectangle described
            through the rectangle the top left edge at (abs_x1, abs_y1)
            and bottom right edge at (abs_x2, abs_y2).
            The ellipse will be filled out if the last parameter is true
        @param abs_x1 X-coordinate of bounding rectangle's top left corner
        @param abs_y1 Y-Coordinate of bounding rectangle's top left corner
        @param abs_x2 X-coordinate of bounding rectangle's bottom right corner
        @param abs_y2 Y-Coordinate of bounding rectangle's bottom right corner
        @param filled Specifies whether the ellipse will be drawn outlined (false) or filled (true)
    */
    virtual void ellipse(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2,
                         const ux_bool &filled) = 0;

    /** Draws an ellipse defined by a CGUIRect
        @see CGfxWrap::ellipse(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2)
        @param abs_rect CGUIRect containing the rectangle corners' endpoints
        @param filled Specifies whether the ellipse will be drawn outlined (false) or filled (true)
    */
    inline void ellipse(const geo::float_rect &abs_rect, const ux_bool &filled)
    {
        ellipse(abs_rect.get_x1(), abs_rect.get_y1(), abs_rect.get_y2(), abs_rect.get_y2(), filled);
    }

    /** Draws the given Text at (abs_x1, abs_y1)
        @param abs_x1 X-position where the Text will be drawn
        @param abs_y1 Y-position where the Text will be drawn
        @param text_string text to output
    */
    void text(const ux_value &abs_x1, const ux_value &abs_y1, const ux_string *const text_string)
    {
//        if (ms_pkFntWrap != NULL)
//        {
////            CONTROLPOINTSTART(CGUIPerfMon::eText)
//            ms_pkFntWrap->text(abs_x1, abs_y1, text_string);
////            CONTROLPOINTEND(CGUIPerfMon::eText)
//        }
    }

    // ---------------------------------------------------------------
    // Clipping and invalidated rectangles
    // ---------------------------------------------------------------

    /** Redraw the invalidated regions of the screen and refresh the visible display of the GUI.
    This method is declared virtual so that you may implement specialized behaviour in derived classes,
    for example if you are using a graphics engine which forces you to flip the front and back buffers and
    you are thus required to disable the Invalidated-rectangles approach. */
    virtual void redraw();

    /** Sets a clipping rectangle.
        All drawing operations will be limited to the region within the clipping rectangle. This is very
        useful for advanced features such as soft-scrolling. Note that the clipping rectangle is not
        allowed to exceed the invalidated rectangle, since this would lead to visual artifacts.
        @param new_abs_cliprect A normalised rectangle that includes all pixels that could be drawn
            by the functions of the graphics wrapper. The rectangle includes the
            border. All pixels outside will be left untouched.
    */
    virtual void set_cliprect(const geo::float_rect &new_abs_cliprect) = 0;

    /// Resets the clipping rectangle
    virtual void reset_cliprect();

    /// Retrieves the currently set clipping rectangle
    virtual void get_cliprect(geo::float_rect &abs_cliprect) const;

    /** Invalidates a region of the GUI.
        All objects overlapping with an invalidated region will be redrawn during the next redraw-cycle.
        The right edge and the bottom edge of the rectangle is not included in the invalidation.
        e.g. invalidate_rect(0, 0, 10, 10 ); will invalidate from (0,0) to (9,9)
            on the screen (both included).
        So invalidate_rect(CGUIObject::GetAbsRect()) will invalidate the complete drawing area of the object.
        @see CGUIObject::GetAbsRect().
        @param invalid_rect Rectangular region of the GUI which has to be redrawn
    */
    void invalidate_rect(const geo::float_rect &invalid_rect);

    /** Invalidates a region of the GUI.
        @see CGfxWrap::invalidate_rect(const CGUIRect &invalid_rect)
        @param abs_x1 X-Coordinate of region's top left corner
        @param abs_y1 Y-Coordinate of region's top left corner
        @param abs_x2 X-Coordinate of region's bottom right corner
        @param abs_y2 Y-Coordinate of region's bottom right corner
    */
    void invalidate_rect(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2, const ux_value &abs_y2)
    {
        invalidate_rect(geo::float_rect(abs_x1, abs_y1, abs_x2, abs_y2));
    }

    /// Returns the current invalidated rectangle
    inline const geo::float_rect &get_invalidated_rect() const
    {
        return m_InvalidatedRect;
    }

    /** Called for each invalidated rectangle right before CGUI::HandleDraw()
      is called.
      A derived graphics wrapper can implement this method to do necessary
      clean-up if required before the GUI tree is drawn. Since HandleDraw
      will only affect the current invalidated rectangle, this method should
      also restrict its operations to that region of the screen.
  */
    virtual void clear_invalid_rect() {}

    // ---------------------------------------------------------------
    // Image-access
    // ---------------------------------------------------------------
    /** Blit an image.
        Blits (draws) the image associated with the supplied Image-identifier to the specified position
        on the screen.
        @param id       Identifier of the image to be blitted.
        @param abs_dest_x Destination x-position.
        @param abs_dest_y Destination y-position.
    */
    void blit_img(const image_id &id, const ux_value &abs_dest_x, const ux_value &abs_dest_y)
    {
        blit_img_ext(id, 0, 0, get_img_width(id), get_img_height(id), abs_dest_x, abs_dest_y, get_img_width(id),
                     get_img_height(id));
    }

    /** Extended blit of an image.
        Performs a stretch blit of the image associated with the supplied Image Identifier.
        The image will be blitted to the position specified by the x- and y-parameter and automatically
        stretched to meet the desired width and height if the stretch flag is set to true.
        @param id       Identifier of the image to be blitted.
        @param abs_dest_x Destination x-position.
        @param abs_dest_y Destination y-position.
        @param width    Target width. Image will be automatically stretched if necessary.
        @param height   Target height. Image will be automatically stretched if necessary.
        @param stretch  Image will be stretched to meet the desired size if set to true.
        @param alpha   Degree of transparency. 0=transparent 255=opaque  Default is opaque.
    */
    void blit_img_ext(const image_id &id, const ux_value &abs_dest_x, const ux_value &abs_dest_y,
                      const ux_value &width, const ux_value &height, const ux_bool &stretch = true,
                      const ux_ubyte &alpha = 255)
    {
        if (id != DUMMY_IMAGE)
        {
            if( stretch)
            {
                blit_img_ext(id, 0, 0, get_img_width(id), get_img_height(id), abs_dest_x, abs_dest_y, width, height,
                             alpha);
            }
            else
            {
                blit_img_ext(id, 0, 0, std::min(static_cast<ux_float> (get_img_width(id)), width),
                             std::min(static_cast<ux_float> (get_img_height(id)), height), abs_dest_x, abs_dest_y,
                             std::min(static_cast<ux_float> (get_img_width(id)), width),
                             std::min(static_cast<ux_float> (get_img_height(id)), height), alpha);
            }
        }
    }

    /** Nine-Patch blit of an image.
        Performs an intelligent 9-patch stretch blit of the image associated with the supplied Image Identifier.
        The image will be blitted to the position given by the upper left corner of the supplied rectangle and stretched
        to the desired width and height. To reduce visual artifacts during stretching, the image will automatically be split
        up into 9 parts. The four corners will be left unscaled, the four edges are each scaled in one axis, and the middle part
        is stretched in both axes. The two parameters uiXBorder and uiYBorder specify the width/height of the sections in pixels.
        @param id              Identifier of the image to be blitted.
        @param abs_rect         Rectangle describing the destination position and size of the image on the screen
        @param top_border      Height of the topmost three sections of the Nine-patch in pixels
        @param bottom_border   Height of the lowermost three sections of the Nine-patch in pixels
        @param left_border     Width of the leftmost three sections of the Nine-patch in pixels
        @param right_border    Width of the rightmost three sections of the Nine-patch in pixels
        @param alpha          Degree of transparency. 0=transparent 255=opaque.  Default is opaque.
    */
    void blit_img_nine_patch(const image_id &id, const geo::float_rect &abs_rect, const ux_uint top_border,
                             const ux_uint bottom_border, const ux_uint left_border, const ux_uint right_border,
                             const ux_ubyte &alpha = 255);

    /** Nine-Patch blit of an image.
        @see blit_img_nine_patch(const image_id &id, const CGUIRect& abs_rect, const ux_uint top_border, const ux_uint bottom_border, const ux_uint left_border, const ux_uint right_border, const ux_ubyte &alpha)
        @param id              Identifier of the image to be blitted.
        @param abs_rect         Rectangle describing the destination position and size of the image on the screen
        @param kNinePatch       Nine-Patch describing the borders
        @param alpha          Degree of transparency. 0=transparent 255=opaque.  Default is opaque.
    */
//    void blit_img_nine_patch(
//        const image_id &id,
//        const geo::float_rect& abs_rect,
//        const CGUINinePatch& kNinePatch,
//        const ux_ubyte &alpha = 255)
//    {
//        blit_img_nine_patch(id, abs_rect, kNinePatch.GetTop(), kNinePatch.GetBottom(), kNinePatch.GetLeft(), kNinePatch.GetRight(), alpha );
//    }

    /** Extended blit of an image.
        Performs a stretch blit of the image associated with the supplied Image Identifier.
        The image will be blitted to the position given by the upper left corner of the supplied rectangle.
        It will be be stretched to meet the desired width and height if the stretch flag is set to true.
        @param id       Identifier of the image to be blitted.
        @param abs_rect  Rectangle describing the destination position and size of the image on the screen
        @param stretch  Image will be stretched to meet the desired size if set to true.
        @param alpha   Degree of transparency. 0=transparent 255=opaque  Default is opaque.
    */
    inline void blit_img_ext(const image_id &id, const geo::float_rect &abs_rect, const ux_bool &stretch = true,
                             const ux_ubyte &alpha = 255)
    {
        blit_img_ext(id, abs_rect.get_x1(), abs_rect.get_y1(), abs_rect.get_width(), abs_rect.get_height(), stretch,
                     alpha);
    }

    /** Extended blit of an image.
        Performs a stretch blit of the image associated with the supplied Image Identifier.
        An area of the image (which is defined by src_abs_rect) will be blitted to the position given by the upper left
        corner of dst_abs_rect. It will be be stretched to meet the desired width and height.
        @param id          Identifier of the image to be blitted.
        @param src_abs_rect  Rectangle describing the relevant area within the source image
        @param dst_abs_rect  Rectangle describing the destination position and size of the image on the screen
        @param alpha      Degree of transparency. 0=transparent 255=opaque  Default is opaque.
    */
    inline void blit_img_ext(const image_id &id, const geo::float_rect &src_abs_rect,
                             const geo::float_rect &dst_abs_rect, const ux_ubyte &alpha = 255)
    {
        blit_img_ext(id, src_abs_rect.get_x1(), src_abs_rect.get_y1(), src_abs_rect.get_width(), src_abs_rect.get_height(),
                     dst_abs_rect.get_x1(), dst_abs_rect.get_y1(), dst_abs_rect.get_width(), dst_abs_rect.get_height(),
                     alpha);
    }

    /** Extended blit of an image.
        Performs a rotated stretch blit of the image associated with the supplied Image Identifier.
        An area of the image (which is defined by abs_src_x, abs_src_y, src_width and src_height) will be blitted to the
        position given by abs_dest_x and abs_dest_y. It will be be stretched to meet dest_width and dest_height if the stretch
        flag is set to true.
        Optionally the image can be rotated by angle degrees around a point defined by rot_center_x and rot_center_y.
        The center of rotation is given in coordinates within a range of 0 to 1 (similar to U/V coordinates), where (0,0) is the
        image's top left corner, (1,1) equals the lower right corner and (0.5, 0.5) corresponds to the image's center.

        @param id          Identifier of the image to be blitted.
        @param abs_src_x    X coordinate of upper left corner of the relevant area within the source image
        @param abs_src_y    Y coordinate of upper left corner of the relevant area within the source image
        @param src_width   Width of the of the relevant area within the source image
        @param src_height  Height of the of the relevant area within the source image
        @param abs_dest_x    Destination X position in absolute coordinates
        @param abs_dest_y    Destination Y position in absolute coordinates
        @param dest_width   Destination width
        @param dest_height  Destination height
        @param alpha      Degree of transparency. 0=transparent 255=opaque  Default is opaque.
        @param angle       Angle in degrees by which the image will be rotated
        @param rot_center_x  Rotation center X. In range of 0 to 1  (like U/V coordinates)
        @param rot_center_y  Rotation center Y. In range of 0 to 1  (like U/V coordinates)
        */
    inline void blit_img_ext(const image_id &id, const ux_uint &abs_src_x, const ux_uint &abs_src_y,
                             const ux_uint &src_width, const ux_uint &src_height, const ux_value &abs_dest_x,
                             const ux_value &abs_dest_y, const ux_value &dest_width, const ux_value &dest_height,
                             const ux_ubyte &alpha = 255, const ux_value &angle = 0, const ux_value &rot_center_x = 0.5,
                             const ux_value &rot_center_y = 0.5)
    {
//        if (MAP_IMGID_TO_ARRAY(id) >= m_uiNOFImages)
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrap::blit_img_ext: Illegal image ID (" + ux_string(id) + ") supplied.\n");
//            return;
//        }

        if (id == DUMMY_IMAGE)
        {
            //   GUILOG(GUI_TRACE_ERROR, "CGfxWrap::blit_img_ext: Tried to access DUMMY_IMAGE. ImageResource is not set.\n");
            return;
        }

//        CGUIImageCache::NotifyImageAccess(id);

        if( !image_exists(id) )
        {
            return;
        }
        //The value of alpha is multiplied by the scaling factor (in the event of, for example, widgets in a composite object)
        blit_img_ext_impl(id, abs_src_x, abs_src_y, src_width, src_height, abs_dest_x, abs_dest_y, dest_width,
                          dest_height, apply_global_alpha(alpha), angle, rot_center_x, rot_center_y);
    }

    /** Returns the width of an image.
        This method first checks for validity of the ID, then calls
        get_img_width_impl().
        @param id Identifier of the image.
        @return The width of the image in pixels.
    */
    inline ux_uint get_img_width(const image_id &id) const
    {
//        if (MAP_IMGID_TO_ARRAY(id) >= m_uiNOFImages)
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrap::get_img_width: Illegal image ID (" + ux_string(id) + ") supplied.\n");
//            return 0;
//        }

        if (id == DUMMY_IMAGE)
        {
            //  GUILOG(GUI_TRACE_WARNING, "CGfxWrap::get_img_width: Tried to access DUMMY_IMAGE. ImageResource is not set.\n");
            return 0;
        }

//        CGUIImageCache::NotifyImageAccess(id);

        if( !image_exists(id) )
        {
            return 0;
        }


        return get_img_width_impl(id);
    }

    /** Gets the height of an image.
        This method first checks for validity of the ID, then calls
        get_img_height_impl().
        @param id Identifier of the image.
        @return The height of the image in pixels.
    */
    inline ux_uint get_img_height(const image_id &id) const
    {
//        if (MAP_IMGID_TO_ARRAY(id) >= m_uiNOFImages)
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrap::get_img_height: Illegal image ID (" + ux_string(id) + ") supplied.\n");
//            return 0;
//        }

        if (id == DUMMY_IMAGE)
        {
            //  GUILOG(GUI_TRACE_ERROR, "CGfxWrap::get_img_height: Tried to access DUMMY_IMAGE. ImageResource is not set.\n");
            return 0;
        }

//        CGUIImageCache::NotifyImageAccess(id);

        if( !image_exists(id) )
        {
            return 0;
        }

        return get_img_height_impl(id);
    }

    /// Returns the bounding box of the image, placed at the origin (0, 0)
    inline geo::float_rect get_img_origin_rect(const image_id &id) const
    {
        return geo::float_rect(0, 0, get_img_width(id), get_img_height(id));
    }

    /** Gets the current foreground color.
        @return The foreground color.
     */
    inline ux_uint get_foreground_color() const {return m_uiColor;}

    /** Gets the current alpha value.
        @return The alpha value.
     */
    inline ux_ubyte get_global_alpha() const {return m_alpha;}

    /** Applies the current global alpha to the given alpha value and returns the result.
        If e.g. the global alpha is 127, and the supplied alpha is 127, the returned value will be 64.
        @param alpha The alpha value to which global alpha shall be applied.
        @return The modified alpha value.
     */
    inline ux_ubyte apply_global_alpha(const ux_ubyte alpha) const {return static_cast<ux_ubyte>(alpha *
            m_global_alpha_factor);}

    /** Gets the number of images (array size).
        @return Number of images.
    */
    inline virtual ux_uint get_nof_images() {return m_uiNOFImages;}

    /** Returns whether the image with the given ID currently is loaded within the Graphics Wrapper.
        This interface must be implemented by derived graphics wrappers.
        @param id The ID to check
        @return True if the image with the given ID is currently loaded, False otherwise.
    */
    virtual ux_bool image_exists(const image_id id) const = 0;


    /** helper method for calculating the next power of two _very_ fast.
        @param vValue eC_value for which the next power of two will be calculated
        @return Returns the closest power-of-two number greater or equal to vValue.
                If NON_POWER_OF_2_TEXTURES is defined, it will return the rounded up
                integer value of vValue.
                Will return 0 when n = 0 and 1 when n = 1.
    */
    inline static ux_uint get_next_power_of_2(const ux_value &vValue)
    {
#if defined NON_POWER_OF_2_TEXTURES
        return eC_ToInt(eC_Ceil(vValue));
#else
        ux_uint pow_of = vValue;
        --pow_of;
        pow_of |= pow_of >> 16;
        pow_of |= pow_of >> 8;
        pow_of |= pow_of >> 4;
        pow_of |= pow_of >> 2;
        pow_of |= pow_of >> 1;
        ++pow_of;
        return pow_of;
#endif
    }

    /** Tell Guiliani that the underlying graphics API enforces flipping the back and front buffers (Instead of copying from the back to the front buffer).
        @param bDoubleBufferingEnforcesFlipping Set to True if flipping of buffers leads to visual artifacts */
    void set_doublebuffering_enforces_flipping(const ux_bool doublebuffering_enforces_flipping) { m_doublebuffering_enforces_flipping = doublebuffering_enforces_flipping; }
    /** Returns whether the DoubleBufferingEnforcesFlipping-flag has been set.
        @return The current status of the DoubleBufferingEnforcesFlipping-flag (as set by the user, or marked as default by the chosen graphics wrapper) */
    ux_bool set_doublebuffering_enforces_flipping() { return m_doublebuffering_enforces_flipping; }

protected:
    /** Method to bring the screen content onto the physical screen so that it becomes
        visible, usually via a blit operation.
        @param eRefreshCallType The call type of this refresh
        @return True of the refresh was performed, False otherwise
    */
    virtual ux_bool refresh(const refresh_type eRefreshCallType) = 0;

    /** Called for every single InvalidatedRect() during redraw().
        Also serves as proxy function to make GETGUI.HandleDraw() accessible from derived graphics wrappers.
        Make sure to call CGfxWrap::start_handle_draw() when overriding this function.
        @param crkClipRect Rectangle against which this cycle of handle draw calls will be clipped. */
    virtual void start_handle_draw(const geo::float_rect &crkClipRect);

    /** Callback which is called at the beginning of redraw()
        Note that this function as well as end_redraw() will also get called if the list of invalidated rects is empty.
        Check m_kInvalidatedRectList if you need more details on the number and dimensions of invalidated areas. */
    virtual void start_redraw() {}

    /// Callback which is called at the end of redraw() after all Invalidated areas were drawn and refreshed
    virtual void end_redraw() {}

    /// Method to clean and release the screen which was created via the create_screen Method
    virtual void destroy_screen() {}

    /** Sets the total number of images. This is called internally by the ResourceManager when dynamically
        allocated image resources require the total number of images to be raised. This calls the set_nof_imagesImpl()
        method internally, which should be implemented in derived GfxWrappers.
        @param uiNOFImages The number of images
        @return True if internal storage has been successfully adapted to support the requested
                number of images. False otherwise.*/
    ux_bool set_nof_images(const ux_uint uiNOFImages)
    {
        m_gp_wrap_semaphore.notify();
        ux_bool bRet = set_nof_imagesImpl(uiNOFImages);
        m_gp_wrap_semaphore.wait();
        return bRet;
    }

    /** Sets the total number of images. This is called internally by the ResourceManager when dynamically
        allocated image resources require the total number of images to be raised. Derived classes will
        have to override this method and resize their internal image-storage structures according to
        the new number of images which will have to be stored.
        @param uiNOFImages The number of images
        @return True if internal storage has been successfully adapted to support the requested
                number of images. False otherwise.*/
    virtual ux_bool set_nof_imagesImpl(const ux_uint uiNOFImages);

    /** Loads an image.
        This loads an image from the given path and associates it with an Image-Identifier.
        This calls the load_img_impl() method internally, which has to be implemented by every GfxWrapper.
        @param kPath Complete path and filename of the image to be loaded.
        @param id ID by which the image will later be accessible.
    */
    void load_img(const ux_string &kPath, const image_id &id)
    {
//        CONTROLPOINTSTART(CGUIPerfMon::eLoadImg)
        m_gp_wrap_semaphore.notify();
        // Only load, if this ImageID is not already in use.
        if( !image_exists(id))
        {
            load_img_impl(kPath, id);
        }
           m_gp_wrap_semaphore.wait();
//        CONTROLPOINTEND(CGUIPerfMon::eLoadImg)
    }

    /** Loads an image.
        This loads an image from the given path and associates it with an Image-Identifier.
        @param kPath Complete path and filename of the image to be loaded.
        @param id ID by which the image will later be accessible.
    */
    virtual void load_img_impl(const ux_string &kPath, const image_id &id) = 0;

    /** Unloads an image.
        Unloads the image associated with the supplied Image-identifier and frees the memory formerly
        occupied by this image.
        @param id Identifier of the image to be unloaded.
    */
    virtual void unload_img(const image_id &id) {}

    /** Computes the size of a loaded image's raw pixel data (e.g. RGBA or RGB565, depending on platform)
        in memory. Needed for the Image Cache.
        Since this will be called by the Guiliani Image cache, derived wrappers _must not_ call
        NotifyImageAccess() from within this method.
        @param id The image's ID.
        @return the size of the image in bytes
    */
    virtual ux_uint get_image_size(const image_id &id) const = 0;

    /** Returns the width of an image. Must be implemented by each graphics
        wrapper.
        @param id The ID of the image whose width should be returned.
                   It is ensured that id is always in the valid range and not
                   DUMMY_IMAGE.
        @return The width of the image in pixels.
      */
    virtual ux_uint get_img_width_impl(const image_id &id) const = 0;

    /** Returns the height of an image. Must be implemented by each graphics
        wrapper.
        @param id The ID of the image whose width should be returned.
                   It is ensured that id is always in the valid range and not
                   DUMMY_IMAGE.
        @return The width of the image in pixels.
      */
    virtual ux_uint get_img_height_impl(const image_id &id) const = 0;

    /** Blits an image. This MUST be implemented by all graphics wrappers.
        It is ensured that the supplied image ID is in the valid range and is not DUMMY_IMAGE.
        For detailed explanations, please refer to CGfxWrap::blit_img_ext

        @param id          Identifier of the image to be blitted.
        @param abs_src_x    X coordinate of upper left corner of the relevant area within the source image
        @param abs_src_y    Y coordinate of upper left corner of the relevant area within the source image
        @param src_width   Width of the of the relevant area within the source image
        @param src_height  Height of the of the relevant area within the source image
        @param abs_dest_x    Destination X position in absolute coordinates
        @param abs_dest_y    Destination Y position in absolute coordinates
        @param dest_width   Destination width
        @param dest_height  Destination height
        @param alpha      Degree of transparency. 0=transparent 255=opaque  Default is opaque.
        @param angle       Angle in degrees by which the image will be rotated
        @param rot_center_x  Rotation center X. In range of 0 to 1  (like U/V coordinates)
        @param rot_center_y  Rotation center Y. In range of 0 to 1  (like U/V coordinates)
      */
    virtual void blit_img_ext_impl(const image_id &id, const ux_uint &abs_src_x, const ux_uint &abs_src_y,
                                   const ux_uint &src_width, const ux_uint &src_height, const ux_value &abs_dest_x,
                                   const ux_value &abs_dest_y, const ux_value &dest_width, const ux_value &dest_height,
                                   const ux_ubyte &alpha = 255, const ux_value &angle = 0,
                                   const ux_value &rot_center_x = 0.5, const ux_value &rot_center_y = 0.5) = 0;


    /** Sets the foreground color. This MUST be implemented by all graphics wrappers.
    For detailed explanations, please refer to CGfxWrap::set_foreground_color
        @param red Red intensity within a range of 0 - 255
        @param green Green intensity within a range of 0 - 255
        @param blue Blue intensity within a range of 0 - 255
        @param alpha Alpha intensity within a range of 0 (transparent) - 255 (opaque)
    */
    virtual void set_foreground_color_impl(const ux_ubyte &red, const ux_ubyte &green, const ux_ubyte &blue,
                                           const ux_ubyte &alpha = 255) = 0;

    /** Sets the current invalidated (dirty) rectangle. This marks the region which has to be redrawn
        during the next call to refresh()
        @param abs_x1 X-Coordinate of top left corner
        @param abs_y1 Y-Coordinate of top left corner
        @param abs_x2 X-Coordinate of bottom right corner
        @param abs_y2 Y-Coordinate of bottom right corner
    */
    void set_invalidated_rect(const ux_value &abs_x1, const ux_value &abs_y1, const ux_value &abs_x2,
                              const ux_value &abs_y2);

    /** Sets the current invalidated (dirty) rectangle. This marks the region which has to be redrawn
        during the next call to refresh()
        @param NewAbsRect Structure of type CGUIRect, specifying the new invalidated rectangle's extends.
    */
    void set_invalidated_rect(const geo::float_rect &NewAbsRect)
    {
        set_invalidated_rect(NewAbsRect.get_x1(), NewAbsRect.get_y1(), NewAbsRect.get_x2(), NewAbsRect.get_y2());
    }

    /// Resets the invalidated rectangle.
    void reset_invalidated_rect();

    /// This is the list of invalidated rectangular regions within the GUI.
//    eC_TListDoubleLinked<geo::float_rect> m_kInvalidatedRectList;

    /// This is the list of regions which have been invalidated during the LAST frame
//    eC_TListDoubleLinked <geo::float_rect> m_kLastFrameInvalidatedRectList;

    /** Guiliani's smart redraw mechanism requires that the back buffer gets COPIED to the front buffer and NOT FLIPPED
        If the underlying graphics API enforces flipping, though, you can set the m_doublebuffering_enforces_flipping flag
        and Guiliani will react accordingly. */
    ux_bool m_doublebuffering_enforces_flipping;

    /** This rectangle marks the region of the screen, which is currently being redrawn.
    During a redraw of the GUI, this will be assigned every rectangle within m_kInvalidetedRectList    */
    geo::float_rect m_InvalidatedRect;

    /** The current clipping rect. Note that this will usually have to be set within the
    used graphics library as well. */
    geo::float_rect m_ClippingRect;

    /// line width in pixels
    ux_value m_line_width;

    /// total number of image-objects
    ux_uint m_uiNOFImages;

    /// Currently set foreground color
    ux_uint m_uiColor;

    /// Currently set global alpha value
    ux_ubyte m_alpha;

    /** Current scaling factor for global alpha
        Used as a helper variable when scaling concatenated alpha operations. */
    ux_value m_global_alpha_factor;
    /**
        Indicates whether HandleDraw() was called in last frame.
        In this case we need a refresh if we use double buffering (to switch buffers)
        even if there were no changes in this frame.
    */
    ux_bool m_bInvalidationInLastFrame;

    /// Semaphore to lock access to methods used in a multi thread context
    ux::semaphore m_gp_wrap_semaphore;

private:
    ///Removes all invalidated areas.
    void reset_invalidation();

    /** Copy-constructor. Should not be used.
        Dummy declaration with no implementation, just to hide the function.
        @param kSource Source object to be copied.
    */
    gp_wrap(const gp_wrap& kSource);

    /** Operator= method. Should not be used.
        Dummy declaration with no implementation, just to hide the function.
        @param kSource Source object to be copied.
        @return This instance.
    */
    gp_wrap& operator=(const gp_wrap& kSource);

    /** Whether a refresh should be forced when the next frame is drawn.
        @see force_refresh()
    */
    ux_bool m_force_refresh;

    /// rect stored during bitmap rendering
    geo::float_rect m_real_invalidated_rect;
    geo::float_rect m_real_cliprect;

    /// Indicates if the GUI is currently being redrawn. While a redraw is in progress e.g. Invalidation will be disabled.
    ux_bool m_currently_redrawing;
};


#endif //UX_GP_WRAP_H
