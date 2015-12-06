//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_GP_WRAP_H
#define UX_GP_WRAP_H

#include "geo_rectangle.h"
#include "ux_types_and_defs.h"

enum ImageResource_t {DUMMY_IMAGE};

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
        @param uiWidth New screen width.
        @param uiHeight New screen height.  */
    virtual void set_screen_size(const ux_uint &uiWidth, const ux_uint &uiHeight);

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
        @param uiCol 32Bit Color value in the Format 0xAARRGGBB
    */
    void set_foreground_color(const ux_uint &uiCol)
    {
        // The value of alpha is multiplied by the scaling factor (in the event of widgets with text, the text's alpha has to be scaled)
        set_foreground_color_impl(GET_R_VAL32(uiCol), GET_G_VAL32(uiCol), GET_B_VAL32(uiCol),
                                  apply_global_alpha(GET_A_VAL32(uiCol)));
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
        m_ubAlpha= apply_global_alpha(alpha);
        m_vGlobalAlphaFactor= m_ubAlpha / 255.0f;
    }


    /** Sets the global alpha via an unsigned Byte.
        In contrast to UpdateGlobalAlpha, this will NOT blend subsequent alphas
        @see update_global_alpha
        @param alpha unsigned Byte
    */
    void set_global_alpha(const ux_ubyte &alpha)
    {
        m_ubAlpha= alpha;
        m_vGlobalAlphaFactor= m_ubAlpha / 255.0f;
    }

    /** Sets the line width in pixels
        @param vWidth width of lines
        @return The previous line width.
    */
    virtual ux_value set_line_width(const ux_value &vWidth)
    {
        ux_value vOldWidth = m_vLineWidth;
        m_vLineWidth = vWidth;
        return vOldWidth;
    }

    /** Draws a line from (vAbsX1, vAbsY1) to (vAbsX2, vAbsY2)
        @param vAbsX1 X-coordinate of starting point
        @param vAbsY1 Y-Coordinate of starting point
        @param vAbsX2 X-coordinate of ending point
        @param vAbsY2 Y-Coordinate of ending point
    */
    virtual void line(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2) = 0;

    /** Draws a line defined by a CGUIRect
        @param kAbsRect CGUIRect containing the line endpoints
        @see CGfxWrap::line(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2)
    */
    inline void line(const geo::FloatRect &kAbsRect)
    {
        line(kAbsRect.get_x1(), kAbsRect.get_y1(), kAbsRect.get_y2(), kAbsRect.get_y2());
    }

    /** Draws a rectangle with the top left edge at (vAbsX1, vAbsY1) and
            bottom right edge at (vAbsX2, vAbsY2).
        The bottom edge and the right edge is not included in the drawn rect.
        e.g. rect(0, 0, 10, 10 ); the rect is drawn from (0,0) to (9,9)
        (both included).
        @see CGUIObject::GetAbsRect()
        @param vAbsX1 X-coordinate of top left corner
        @param vAbsY1 Y-Coordinate of top left corner
        @param vAbsX2 X-coordinate of bottom right corner
        @param vAbsY2 Y-Coordinate of bottom right corner
    */
    virtual void rect(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2) = 0;

    /** Draws a rectangle defined by a CGUIRect
        @param kAbsRect CGUIRect containing the rectangle corners' endpoints
        @see CGfxWrap::rect(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2)
    */
    inline void rect(const geo::FloatRect &kAbsRect)
    {
        rect(kAbsRect.get_x1(), kAbsRect.get_y1(), kAbsRect.get_y2(), kAbsRect.get_y2());
    }

    /** Draws a filled rectangle with the top left edge at (vAbsX1, vAbsY1)
            and bottom right edge at (vAbsX2, vAbsY2)
        The bottom edge and the right edge is not included in the drawn rect.
        e.g. filled_rect(0, 0, 10, 10 ); the rect is drawn from (0,0) to (9,9)
        (both included).
        @see CGUIObject::GetAbsRect()
        @param vAbsX1 X-coordinate of top left corner
        @param vAbsY1 Y-Coordinate of top left corner
        @param vAbsX2 X-coordinate of bottom right corner
        @param vAbsY2 Y-Coordinate of bottom right corner
    */
    virtual void filled_rect(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2,
                             const ux_value &vAbsY2) = 0;

    /** Draws a filled rectangle defined by a CGUIRect
        @param kAbsRect CGUIRect containing the rectangle corners' endpoints
        @see CGfxWrap::filled_rect(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2)
    */
    inline void filled_rect(const geo::FloatRect &kAbsRect)
    {
        filled_rect(kAbsRect.get_x1(), kAbsRect.get_y1(), kAbsRect.get_y2(), kAbsRect.get_y2());
    }

    /** Draws an ellipse which is complete inside the rectangle described
            through the rectangle the top left edge at (vAbsX1, vAbsY1)
            and bottom right edge at (vAbsX2, vAbsY2).
            The ellipse will be filled out if the last parameter is true
        @param vAbsX1 X-coordinate of bounding rectangle's top left corner
        @param vAbsY1 Y-Coordinate of bounding rectangle's top left corner
        @param vAbsX2 X-coordinate of bounding rectangle's bottom right corner
        @param vAbsY2 Y-Coordinate of bounding rectangle's bottom right corner
        @param bFilled Specifies whether the ellipse will be drawn outlined (false) or filled (true)
    */
    virtual void ellipse(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2,
                         const ux_bool &bFilled) = 0;

    /** Draws an ellipse defined by a CGUIRect
        @see CGfxWrap::ellipse(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2)
        @param kAbsRect CGUIRect containing the rectangle corners' endpoints
        @param bFilled Specifies whether the ellipse will be drawn outlined (false) or filled (true)
    */
    inline void ellipse(const geo::FloatRect &kAbsRect, const ux_bool &bFilled)
    {
        ellipse(kAbsRect.get_x1(), kAbsRect.get_y1(), kAbsRect.get_y2(), kAbsRect.get_y2(), bFilled);
    }

    /** Draws the given Text at (vAbsX1, vAbsY1)
        @param vAbsX1 X-position where the Text will be drawn
        @param vAbsY1 Y-position where the Text will be drawn
        @param lpString text to output
    */
    void text(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_string *const lpString)
    {
//        if (ms_pkFntWrap != NULL)
//        {
////            CONTROLPOINTSTART(CGUIPerfMon::eText)
//            ms_pkFntWrap->text(vAbsX1, vAbsY1, lpString);
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
        @param NewAbsClipRect A normalised rectangle that includes all pixels that could be drawn
            by the functions of the graphics wrapper. The rectangle includes the
            border. All pixels outside will be left untouched.
    */
    virtual void set_cliprect(const geo::FloatRect &NewAbsClipRect) = 0;

    /// Resets the clipping rectangle
    virtual void reset_cliprect();

    /// Retrieves the currently set clipping rectangle
    virtual void get_cliprect(geo::FloatRect &AbsClipRect) const;

    /** Invalidates a region of the GUI.
        All objects overlapping with an invalidated region will be redrawn during the next redraw-cycle.
        The right edge and the bottom edge of the rectangle is not included in the invalidation.
        e.g. invalidate_rect(0, 0, 10, 10 ); will invalidate from (0,0) to (9,9)
            on the screen (both included).
        So invalidate_rect(CGUIObject::GetAbsRect()) will invalidate the complete drawing area of the object.
        @see CGUIObject::GetAbsRect().
        @param kInvalidRect Rectangular region of the GUI which has to be redrawn
    */
    void invalidate_rect(const geo::FloatRect &kInvalidRect);

    /** Invalidates a region of the GUI.
        @see CGfxWrap::invalidate_rect(const CGUIRect &kInvalidRect)
        @param vAbsX1 X-Coordinate of region's top left corner
        @param vAbsY1 Y-Coordinate of region's top left corner
        @param vAbsX2 X-Coordinate of region's bottom right corner
        @param vAbsY2 Y-Coordinate of region's bottom right corner
    */
    void invalidate_rect(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2, const ux_value &vAbsY2)
    {
        invalidate_rect(geo::FloatRect(vAbsX1, vAbsY1, vAbsX2, vAbsY2));
    }

    /// Returns the current invalidated rectangle
    inline const geo::FloatRect&get_invalidated_rect() const
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
        @param eID       Identifier of the image to be blitted.
        @param vAbsDestX Destination x-position.
        @param vAbsDestY Destination y-position.
    */
    void blit_img(const ImageResource_t &eID, const ux_value &vAbsDestX, const ux_value &vAbsDestY)
    {
        blit_img_ext(eID, 0, 0, get_img_width(eID), get_img_height(eID), vAbsDestX, vAbsDestY, get_img_width(eID),
                     get_img_height(eID));
    }

    /** Extended blit of an image.
        Performs a stretch blit of the image associated with the supplied Image Identifier.
        The image will be blitted to the position specified by the x- and y-parameter and automatically
        stretched to meet the desired width and height if the bStretch flag is set to true.
        @param eID       Identifier of the image to be blitted.
        @param vAbsDestX Destination x-position.
        @param vAbsDestY Destination y-position.
        @param vWidth    Target width. Image will be automatically stretched if necessary.
        @param vHeight   Target height. Image will be automatically stretched if necessary.
        @param bStretch  Image will be stretched to meet the desired size if set to true.
        @param alpha   Degree of transparency. 0=transparent 255=opaque  Default is opaque.
    */
    void blit_img_ext(const ImageResource_t &eID, const ux_value &vAbsDestX, const ux_value &vAbsDestY,
                      const ux_value &vWidth, const ux_value &vHeight, const ux_bool &bStretch = true,
                      const ux_ubyte &alpha = 255)
    {
        if (eID != DUMMY_IMAGE)
        {
            if( bStretch)
            {
                blit_img_ext(eID, 0, 0, get_img_width(eID), get_img_height(eID), vAbsDestX, vAbsDestY, vWidth, vHeight,
                             alpha);
            }
            else
            {
                blit_img_ext(eID, 0, 0, std::min(static_cast<ux_float> (get_img_width(eID)), vWidth),
                             std::min(static_cast<ux_float> (get_img_height(eID)), vHeight), vAbsDestX, vAbsDestY,
                             std::min(static_cast<ux_float> (get_img_width(eID)), vWidth),
                             std::min(static_cast<ux_float> (get_img_height(eID)), vHeight), alpha);
            }
        }
    }

    /** Nine-Patch blit of an image.
        Performs an intelligent 9-patch stretch blit of the image associated with the supplied Image Identifier.
        The image will be blitted to the position given by the upper left corner of the supplied rectangle and stretched
        to the desired width and height. To reduce visual artifacts during stretching, the image will automatically be split
        up into 9 parts. The four corners will be left unscaled, the four edges are each scaled in one axis, and the middle part
        is stretched in both axes. The two parameters uiXBorder and uiYBorder specify the width/height of the sections in pixels.
        @param eID              Identifier of the image to be blitted.
        @param kAbsRect         Rectangle describing the destination position and size of the image on the screen
        @param uiTopBorder      Height of the topmost three sections of the Nine-patch in pixels
        @param uiBottomBorder   Height of the lowermost three sections of the Nine-patch in pixels
        @param uiLeftBorder     Width of the leftmost three sections of the Nine-patch in pixels
        @param uiRightBorder    Width of the rightmost three sections of the Nine-patch in pixels
        @param alpha          Degree of transparency. 0=transparent 255=opaque.  Default is opaque.
    */
    void blit_img_nine_patch(const ImageResource_t &eID, const geo::FloatRect &kAbsRect, const ux_uint uiTopBorder,
                             const ux_uint uiBottomBorder, const ux_uint uiLeftBorder, const ux_uint uiRightBorder,
                             const ux_ubyte &alpha = 255);

    /** Nine-Patch blit of an image.
        @see blit_img_nine_patch(const ImageResource_t &eID, const CGUIRect& kAbsRect, const ux_uint uiTopBorder, const ux_uint uiBottomBorder, const ux_uint uiLeftBorder, const ux_uint uiRightBorder, const ux_ubyte &alpha)
        @param eID              Identifier of the image to be blitted.
        @param kAbsRect         Rectangle describing the destination position and size of the image on the screen
        @param kNinePatch       Nine-Patch describing the borders
        @param alpha          Degree of transparency. 0=transparent 255=opaque.  Default is opaque.
    */
//    void blit_img_nine_patch(
//        const ImageResource_t &eID,
//        const geo::FloatRect& kAbsRect,
//        const CGUINinePatch& kNinePatch,
//        const ux_ubyte &alpha = 255)
//    {
//        blit_img_nine_patch(eID, kAbsRect, kNinePatch.GetTop(), kNinePatch.GetBottom(), kNinePatch.GetLeft(), kNinePatch.GetRight(), alpha );
//    }

    /** Extended blit of an image.
        Performs a stretch blit of the image associated with the supplied Image Identifier.
        The image will be blitted to the position given by the upper left corner of the supplied rectangle.
        It will be be stretched to meet the desired width and height if the bStretch flag is set to true.
        @param eID       Identifier of the image to be blitted.
        @param kAbsRect  Rectangle describing the destination position and size of the image on the screen
        @param bStretch  Image will be stretched to meet the desired size if set to true.
        @param alpha   Degree of transparency. 0=transparent 255=opaque  Default is opaque.
    */
    inline void blit_img_ext(const ImageResource_t &eID, const geo::FloatRect &kAbsRect, const ux_bool &bStretch = true,
                             const ux_ubyte &alpha = 255)
    {
        blit_img_ext(eID, kAbsRect.get_x1(), kAbsRect.get_y1(), kAbsRect.get_width(), kAbsRect.get_height(), bStretch,
                     alpha);
    }

    /** Extended blit of an image.
        Performs a stretch blit of the image associated with the supplied Image Identifier.
        An area of the image (which is defined by kSrcAbsRect) will be blitted to the position given by the upper left
        corner of kDstAbsRect. It will be be stretched to meet the desired width and height.
        @param eID          Identifier of the image to be blitted.
        @param kSrcAbsRect  Rectangle describing the relevant area within the source image
        @param kDstAbsRect  Rectangle describing the destination position and size of the image on the screen
        @param alpha      Degree of transparency. 0=transparent 255=opaque  Default is opaque.
    */
    inline void blit_img_ext(const ImageResource_t &eID, const geo::FloatRect &kSrcAbsRect,
                             const geo::FloatRect &kDstAbsRect, const ux_ubyte &alpha = 255)
    {
        blit_img_ext(eID, kSrcAbsRect.get_x1(), kSrcAbsRect.get_y1(), kSrcAbsRect.get_width(), kSrcAbsRect.get_height(),
                     kDstAbsRect.get_x1(), kDstAbsRect.get_y1(), kDstAbsRect.get_width(), kDstAbsRect.get_height(),
                     alpha);
    }

    /** Extended blit of an image.
        Performs a rotated stretch blit of the image associated with the supplied Image Identifier.
        An area of the image (which is defined by uiAbsSrcX, uiAbsSrcY, uiSrcWidth and uiSrcHeight) will be blitted to the
        position given by vAbsDestX and vAbsDestY. It will be be stretched to meet vDestWidth and vDestHeight if the bStretch
        flag is set to true.
        Optionally the image can be rotated by vAngle degrees around a point defined by vRotCenterX and vRotCenterY.
        The center of rotation is given in coordinates within a range of 0 to 1 (similar to U/V coordinates), where (0,0) is the
        image's top left corner, (1,1) equals the lower right corner and (0.5, 0.5) corresponds to the image's center.

        @param eID          Identifier of the image to be blitted.
        @param uiAbsSrcX    X coordinate of upper left corner of the relevant area within the source image
        @param uiAbsSrcY    Y coordinate of upper left corner of the relevant area within the source image
        @param uiSrcWidth   Width of the of the relevant area within the source image
        @param uiSrcHeight  Height of the of the relevant area within the source image
        @param vAbsDestX    Destination X position in absolute coordinates
        @param vAbsDestY    Destination Y position in absolute coordinates
        @param vDestWidth   Destination width
        @param vDestHeight  Destination height
        @param alpha      Degree of transparency. 0=transparent 255=opaque  Default is opaque.
        @param vAngle       Angle in degrees by which the image will be rotated
        @param vRotCenterX  Rotation center X. In range of 0 to 1  (like U/V coordinates)
        @param vRotCenterY  Rotation center Y. In range of 0 to 1  (like U/V coordinates)
        */
    inline void blit_img_ext(const ImageResource_t &eID, const ux_uint &uiAbsSrcX, const ux_uint &uiAbsSrcY,
                             const ux_uint &uiSrcWidth, const ux_uint &uiSrcHeight, const ux_value &vAbsDestX,
                             const ux_value &vAbsDestY, const ux_value &vDestWidth, const ux_value &vDestHeight,
                             const ux_ubyte &alpha = 255, const ux_value &vAngle = 0, const ux_value &vRotCenterX = 0.5,
                             const ux_value &vRotCenterY = 0.5)
    {
//        if (MAP_IMGID_TO_ARRAY(eID) >= m_uiNOFImages)
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrap::blit_img_ext: Illegal image ID (" + ux_string(eID) + ") supplied.\n");
//            return;
//        }

        if (eID == DUMMY_IMAGE)
        {
            //   GUILOG(GUI_TRACE_ERROR, "CGfxWrap::blit_img_ext: Tried to access DUMMY_IMAGE. ImageResource is not set.\n");
            return;
        }

//        CGUIImageCache::NotifyImageAccess(eID);

        if( !image_exists(eID) )
        {
            return;
        }
        //The value of alpha is multiplied by the scaling factor (in the event of, for example, widgets in a composite object)
        blit_img_ext_impl(eID, uiAbsSrcX, uiAbsSrcY, uiSrcWidth, uiSrcHeight, vAbsDestX, vAbsDestY, vDestWidth,
                          vDestHeight, apply_global_alpha(alpha), vAngle, vRotCenterX, vRotCenterY);
    }

    /** Returns the width of an image.
        This method first checks for validity of the ID, then calls
        get_img_width_impl().
        @param eID Identifier of the image.
        @return The width of the image in pixels.
    */
    inline ux_uint get_img_width(const ImageResource_t &eID) const
    {
//        if (MAP_IMGID_TO_ARRAY(eID) >= m_uiNOFImages)
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrap::get_img_width: Illegal image ID (" + ux_string(eID) + ") supplied.\n");
//            return 0;
//        }

        if (eID == DUMMY_IMAGE)
        {
            //  GUILOG(GUI_TRACE_WARNING, "CGfxWrap::get_img_width: Tried to access DUMMY_IMAGE. ImageResource is not set.\n");
            return 0;
        }

//        CGUIImageCache::NotifyImageAccess(eID);

        if( !image_exists(eID) )
        {
            return 0;
        }


        return get_img_width_impl(eID);
    }

    /** Gets the height of an image.
        This method first checks for validity of the ID, then calls
        get_img_height_impl().
        @param eID Identifier of the image.
        @return The height of the image in pixels.
    */
    inline ux_uint get_img_height(const ImageResource_t &eID) const
    {
//        if (MAP_IMGID_TO_ARRAY(eID) >= m_uiNOFImages)
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrap::get_img_height: Illegal image ID (" + ux_string(eID) + ") supplied.\n");
//            return 0;
//        }

        if (eID == DUMMY_IMAGE)
        {
            //  GUILOG(GUI_TRACE_ERROR, "CGfxWrap::get_img_height: Tried to access DUMMY_IMAGE. ImageResource is not set.\n");
            return 0;
        }

//        CGUIImageCache::NotifyImageAccess(eID);

        if( !image_exists(eID) )
        {
            return 0;
        }

        return get_img_height_impl(eID);
    }

    /// Returns the bounding box of the image, placed at the origin (0, 0)
    inline geo::FloatRect get_img_origin_rect(const ImageResource_t &eID) const
    {
        return geo::FloatRect(0, 0, get_img_width(eID), get_img_height(eID));
    }

    /** Gets the current foreground color.
        @return The foreground color.
     */
    inline ux_uint get_foreground_color() const {return m_uiColor;}

    /** Gets the current alpha value.
        @return The alpha value.
     */
    inline ux_ubyte get_global_alpha() const {return m_ubAlpha;}

    /** Applies the current global alpha to the given alpha value and returns the result.
        If e.g. the global alpha is 127, and the supplied alpha is 127, the returned value will be 64.
        @param alpha The alpha value to which global alpha shall be applied.
        @return The modified alpha value.
     */
    inline ux_ubyte apply_global_alpha(const ux_ubyte alpha) const {return static_cast<ux_ubyte>(alpha * m_vGlobalAlphaFactor);}

    /** Gets the number of images (array size).
        @return Number of images.
    */
    inline virtual ux_uint get_nof_images() {return m_uiNOFImages;}

    /** Returns whether the image with the given ID currently is loaded within the Graphics Wrapper.
        This interface must be implemented by derived graphics wrappers.
        @param eID The ID to check
        @return True if the image with the given ID is currently loaded, False otherwise.
    */
    virtual ux_bool image_exists(const ImageResource_t eID) const = 0;


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
        ux_uint uiPOW = vValue;
        --uiPOW;
        uiPOW |= uiPOW >> 16;
        uiPOW |= uiPOW >> 8;
        uiPOW |= uiPOW >> 4;
        uiPOW |= uiPOW >> 2;
        uiPOW |= uiPOW >> 1;
        ++uiPOW;
        return uiPOW;
#endif
    }

    /** Tell Guiliani that the underlying graphics API enforces flipping the back and front buffers (Instead of copying from the back to the front buffer).
        @param bDoubleBufferingEnforcesFlipping Set to True if flipping of buffers leads to visual artifacts */
    void set_doublebuffering_enforces_flipping(const ux_bool bDoubleBufferingEnforcesFlipping) { m_bDoubleBufferingEnforcesFlipping = bDoubleBufferingEnforcesFlipping; }
    /** Returns whether the DoubleBufferingEnforcesFlipping-flag has been set.
        @return The current status of the DoubleBufferingEnforcesFlipping-flag (as set by the user, or marked as default by the chosen graphics wrapper) */
    ux_bool set_doublebuffering_enforces_flipping() { return m_bDoubleBufferingEnforcesFlipping; }

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
    virtual void start_handle_draw(const geo::FloatRect &crkClipRect);

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
//        m_kGfxWrapSemaphore.Enter();
        ux_bool bRet = set_nof_imagesImpl(uiNOFImages);
//        m_kGfxWrapSemaphore.Leave();
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
        @param eID ID by which the image will later be accessible.
    */
    void load_img(const ux_string &kPath, const ImageResource_t &eID)
    {
//        CONTROLPOINTSTART(CGUIPerfMon::eLoadImg)
//        m_kGfxWrapSemaphore.Enter();
        // Only load, if this ImageID is not already in use.
        if( !image_exists(eID))
        {
            load_img_impl(kPath, eID);
        }
//        m_kGfxWrapSemaphore.Leave();
//        CONTROLPOINTEND(CGUIPerfMon::eLoadImg)
    }

    /** Loads an image.
        This loads an image from the given path and associates it with an Image-Identifier.
        @param kPath Complete path and filename of the image to be loaded.
        @param eID ID by which the image will later be accessible.
    */
    virtual void load_img_impl(const ux_string &kPath, const ImageResource_t &eID) = 0;

    /** Unloads an image.
        Unloads the image associated with the supplied Image-identifier and frees the memory formerly
        occupied by this image.
        @param eID Identifier of the image to be unloaded.
    */
    virtual void unload_img(const ImageResource_t &eID) {}

    /** Computes the size of a loaded image's raw pixel data (e.g. RGBA or RGB565, depending on platform)
        in memory. Needed for the Image Cache.
        Since this will be called by the Guiliani Image cache, derived wrappers _must not_ call
        NotifyImageAccess() from within this method.
        @param eID The image's ID.
        @return the size of the image in bytes
    */
    virtual ux_uint get_image_size(const ImageResource_t &eID) const = 0;

    /** Returns the width of an image. Must be implemented by each graphics
        wrapper.
        @param eID The ID of the image whose width should be returned.
                   It is ensured that eID is always in the valid range and not
                   DUMMY_IMAGE.
        @return The width of the image in pixels.
      */
    virtual ux_uint get_img_width_impl(const ImageResource_t &eID) const = 0;

    /** Returns the height of an image. Must be implemented by each graphics
        wrapper.
        @param eID The ID of the image whose width should be returned.
                   It is ensured that eID is always in the valid range and not
                   DUMMY_IMAGE.
        @return The width of the image in pixels.
      */
    virtual ux_uint get_img_height_impl(const ImageResource_t &eID) const = 0;

    /** Blits an image. This MUST be implemented by all graphics wrappers.
        It is ensured that the supplied image ID is in the valid range and is not DUMMY_IMAGE.
        For detailed explanations, please refer to CGfxWrap::blit_img_ext

        @param eID          Identifier of the image to be blitted.
        @param uiAbsSrcX    X coordinate of upper left corner of the relevant area within the source image
        @param uiAbsSrcY    Y coordinate of upper left corner of the relevant area within the source image
        @param uiSrcWidth   Width of the of the relevant area within the source image
        @param uiSrcHeight  Height of the of the relevant area within the source image
        @param vAbsDestX    Destination X position in absolute coordinates
        @param vAbsDestY    Destination Y position in absolute coordinates
        @param vDestWidth   Destination width
        @param vDestHeight  Destination height
        @param alpha      Degree of transparency. 0=transparent 255=opaque  Default is opaque.
        @param vAngle       Angle in degrees by which the image will be rotated
        @param vRotCenterX  Rotation center X. In range of 0 to 1  (like U/V coordinates)
        @param vRotCenterY  Rotation center Y. In range of 0 to 1  (like U/V coordinates)
      */
    virtual void blit_img_ext_impl(const ImageResource_t &eID, const ux_uint &uiAbsSrcX, const ux_uint &uiAbsSrcY,
                                   const ux_uint &uiSrcWidth, const ux_uint &uiSrcHeight, const ux_value &vAbsDestX,
                                   const ux_value &vAbsDestY, const ux_value &vDestWidth, const ux_value &vDestHeight,
                                   const ux_ubyte &alpha = 255, const ux_value &vAngle = 0,
                                   const ux_value &vRotCenterX = 0.5, const ux_value &vRotCenterY = 0.5) = 0;


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
        @param vAbsX1 X-Coordinate of top left corner
        @param vAbsY1 Y-Coordinate of top left corner
        @param vAbsX2 X-Coordinate of bottom right corner
        @param vAbsY2 Y-Coordinate of bottom right corner
    */
    void set_invalidated_rect(const ux_value &vAbsX1, const ux_value &vAbsY1, const ux_value &vAbsX2,
                              const ux_value &vAbsY2);

    /** Sets the current invalidated (dirty) rectangle. This marks the region which has to be redrawn
        during the next call to refresh()
        @param NewAbsRect Structure of type CGUIRect, specifying the new invalidated rectangle's extends.
    */
    void set_invalidated_rect(const geo::FloatRect &NewAbsRect)
    {
        set_invalidated_rect(NewAbsRect.get_x1(), NewAbsRect.get_y1(), NewAbsRect.get_x2(), NewAbsRect.get_y2());
    }

    /// Resets the invalidated rectangle.
    void reset_invalidated_rect();

    /// This is the list of invalidated rectangular regions within the GUI.
//    eC_TListDoubleLinked<geo::FloatRect> m_kInvalidatedRectList;

    /// This is the list of regions which have been invalidated during the LAST frame
//    eC_TListDoubleLinked <geo::FloatRect> m_kLastFrameInvalidatedRectList;

    /** Guiliani's smart redraw mechanism requires that the back buffer gets COPIED to the front buffer and NOT FLIPPED
        If the underlying graphics API enforces flipping, though, you can set the m_bDoubleBufferingEnforcesFlipping flag
        and Guiliani will react accordingly. */
    ux_bool m_bDoubleBufferingEnforcesFlipping;

    /** This rectangle marks the region of the screen, which is currently being redrawn.
    During a redraw of the GUI, this will be assigned every rectangle within m_kInvalidetedRectList    */
    geo::FloatRect m_InvalidatedRect;

    /** The current clipping rect. Note that this will usually have to be set within the
    used graphics library as well. */
    geo::FloatRect m_ClippingRect;

    /// line width in pixels
    ux_value m_vLineWidth;

    /// total number of image-objects
    ux_uint m_uiNOFImages;

    /// Currently set foreground color
    ux_uint m_uiColor;

    /// Currently set global alpha value
    ux_ubyte m_ubAlpha;

    /** Current scaling factor for global alpha
        Used as a helper variable when scaling concatenated alpha operations. */
    ux_value m_vGlobalAlphaFactor;
    /**
        Indicates whether HandleDraw() was called in last frame.
        In this case we need a refresh if we use double buffering (to switch buffers)
        even if there were no changes in this frame.
    */
    ux_bool m_bInvalidationInLastFrame;

    /// Semaphore to lock access to methods used in a multi thread context
//    eC_Semaphore m_kGfxWrapSemaphore;

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
    geo::FloatRect m_kRealInvalidatedRect;
    geo::FloatRect m_kRealClipRect;

    /// Indicates if the GUI is currently being redrawn. While a redraw is in progress e.g. Invalidation will be disabled.
    ux_bool m_bCurrentlyRedrawing;
};


#endif //UX_GP_WRAP_H
