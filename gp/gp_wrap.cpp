//
// Created by Bernd Lappas on 29.11.15.
//

#include "gp_wrap.h"

// Return an unsigned int value from RGB (Red, Green, Blue) color value. R, G and B are expected to be bytes.
ux_uint UINT_FROM_RGB(const ux_ubyte & R, const ux_ubyte & G, const ux_ubyte & B)
{
    return
        (static_cast<ux_uint>(B)) |
            ((static_cast<ux_uint>(G))<<8) |
            ((static_cast<ux_uint>(R))<<16);
}

// Return an unsigned int value from ARGB (Alpha, Red, Green, Blue) color value. A, R, G and B are expected to be bytes.
ux_uint UINT_FROM_ARGB(const ux_ubyte & A, const ux_ubyte & R, const ux_ubyte & G, const ux_ubyte & B)
{
    return
        (static_cast<ux_uint>(B)) |
            ((static_cast<ux_uint>(G))<<8) |
            ((static_cast<ux_uint>(R))<<16) |
            ((static_cast<ux_uint>(A))<<24);
}

/// Return the alpha portion of the parameter.
ux_ubyte GET_A_VAL32(const ux_uint & argb)
{
    return static_cast<ux_ubyte>((argb >> 24) & 0xFF);
}
/// Return the red portion of the parameter.
ux_ubyte GET_R_VAL32(const ux_uint & argb)
{
    return static_cast<ux_ubyte>((argb >> 16) & 0xFF);
}

/// Return the green portion of the parameter.
ux_ubyte GET_G_VAL32(const ux_uint & argb)
{
    return static_cast<ux_ubyte>((argb >> 8) & 0xFF);
}

/// Returns the blue portion of the parameter.
ux_ubyte GET_B_VAL32(const ux_uint & argb)
{
    return static_cast<ux_ubyte>(argb & 0xFF);
}

// Better ? #ifdef !defined(NDEBUG)
#if defined _DEBUG
//These two attributes are compared to asssure that the number redraws and refreshes are equal which is important on some wrappers.
ux_uint g_uiRedrawGUICounter;
ux_uint g_uiRefreshGUICounter;
#endif

gp_wrap::gp_wrap():
    m_bDoubleBufferingEnforcesFlipping(false),
    m_InvalidatedRect(),
    m_ClippingRect(),
    m_vLineWidth(1.0f),
    m_uiNOFImages(0),
    m_uiColor(0),
    m_ubAlpha(255),
    m_vGlobalAlphaFactor(1.0f), //Since the default alpha is 255, the scaling factor has to be 1
    m_bInvalidationInLastFrame(true),
    m_force_refresh(false),
    m_kRealInvalidatedRect(),
    m_kRealClipRect(),
    m_bCurrentlyRedrawing(false)
{
    gp_wrap::reset_invalidated_rect();
    gp_wrap::reset_cliprect();

    // Initialize internal image counter with number of static images
    m_uiNOFImages = 0; //cuiNOFImageResources;
    // Creation of semaphore
    //    m_kGfxWrapSemaphore.Create("GfxWrapSemaphore");

#if defined _DEBUG
    g_uiRedrawGUICounter = 0;
    g_uiRefreshGUICounter = 0;
#endif
}

gp_wrap::~gp_wrap(void)
{
}

ux_bool gp_wrap::init()
{
    return true;
}

void gp_wrap::de_init()
{
    //delete all created and not removed Bitmaps
//    for(ux_uint i = 0; i < m_apBitmaps.GetQuantity(); i++)
//    {
//        delete m_apBitmaps[i];
//        m_apBitmaps[i] = NULL;
//    }
}

void gp_wrap::reset_cliprect()
{
}

void gp_wrap::get_cliprect(geo::FloatRect &ClipRect) const
{
    ClipRect = m_ClippingRect;
}

void gp_wrap::set_cliprect(const geo::FloatRect &NewClipRect)
{
    // The clipping rectangle may not exceed the invalidated region of the screen. Therefore
    // we calculate the intersecting rectangle of the clipping rect with the invalidated rect.
    // Thus a GUIObject will only be drawn if it is invalidated.
    m_ClippingRect = NewClipRect;
    m_ClippingRect.intersects(m_InvalidatedRect);

#ifdef GUILIANI_GFXDEBUG
    GUILOG( GUI_TRACE_DEBUG, ("CGfxWrap::SetCliprect: Clp " +
        ux_string(eC_ToInt(m_ClippingRect.GetX1())) + " " +
        ux_string(eC_ToInt(m_ClippingRect.GetY1())) + " " +
        ux_string(eC_ToInt(m_ClippingRect.GetX2())) + " " +
        ux_string(eC_ToInt(m_ClippingRect.GetY2())) +
        (m_ClippingRect.IsComprehensive() ? ux_string(" comprehensive.\n") : ux_string(" non-comprehensive.\n"))) );
#endif
}

void gp_wrap::reset_invalidated_rect()
{
    m_InvalidatedRect = geo::FloatRect();
}

void gp_wrap::reset_invalidation()
{
//    m_kInvalidatedRectList.RemoveAll();
    reset_invalidated_rect();
}


void gp_wrap::set_invalidated_rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2)
{
    m_InvalidatedRect = geo::FloatRect(vX1, vY1, vX2, vY2);

#ifdef GUILIANI_GFXDEBUG
    GUILOG(GUI_TRACE_DEBUG,"CGfxWrap::set_invalidated_rect"
        ": vX1: "+ux_string(eC_ToInt(m_InvalidatedRect.GetX1())) +
        ", vY1: "+ux_string(eC_ToInt(m_InvalidatedRect.GetY1())) +
        ", vX2: "+ux_string(eC_ToInt(m_InvalidatedRect.GetX2())) +
        ", vY2: "+ux_string(eC_ToInt(m_InvalidatedRect.GetY2())) + ".\n");
#endif
}

ux_bool gp_wrap::set_nof_imagesImpl(const ux_uint uiNOFImages){
//    GUILOG( GUI_TRACE_WARNING,"CGfxWrap::set_nof_imagesImpl: Total number of images is fix for this GfxWrapper. Dynamic images are not supported\n");
//    GUILOG( GUI_TRACE_WARNING," Request for a total of "+ux_string(uiNOFImages)+" images has been ignored. \n");
    return false;
}

void gp_wrap::start_handle_draw(const geo::FloatRect &crkClipRect)
{
//    GETGUI.HandleDraw(crkClipRect);
}

void gp_wrap::redraw()
{
    // Initially reset global alpha to fully opaque
    set_global_alpha(255);

    if (m_force_refresh)
    {
        m_force_refresh = false;
        refresh(gp_wrap::refresh_forced);
    }
    // Call updateInvalidation on the object tree, which will cause the absolute coordinates
    // of all objects to be updated.
//    GETGUI.UpdateInvalidation();

    /*
     The following code section "emulates" partial redraw for systems where "flipping" of the double buffers does
     indeed exchange the front and back buffers - and not just copy the back to the front buffer.
     This is achieved by keeping track of invalidated rects from the previous frame, and invalidating those once again
     within the current frame.
    */
    if( m_bDoubleBufferingEnforcesFlipping)
    {
//        // For intermediate storage during "merging" of this frame's and the last frame's invalidated rects
//        eC_TListDoubleLinked <geo::FloatRect> kTempInvalidatedRectList;
//        // Backup current invalidated rect list
//        kTempInvalidatedRectList = m_kInvalidatedRectList;
//        // Merge lists of invalidated rects, by invalidating all rects form the last frame once again
//        eC_TListDoubleLinked<geo::FloatRect>::SafeIterator sIter;
//        FOR_ALL_FORWARD_SAFE (sIter, m_kLastFrameInvalidatedRectList)
//        {
//            invalidate_rect( *sIter);
//        }
//        // Store this frame's invalidated rect list for next frame
//        m_kLastFrameInvalidatedRectList = kTempInvalidatedRectList;
    }

    // All invalidated rects have been collected. Now call start_redraw() callback
    start_redraw();

    // Do not call redraw if there's nothing to be redrawn
//    if (m_kInvalidatedRectList.IsEmpty() && m_bInvalidationInLastFrame == false)
//    {
//        // Notify that we're done so that number of start_redraw() and end_redraw() calls is equal
//        end_redraw();
//
//        return;
//    }

    // Reset m_bInvalidationInLastFrame flag
    m_bInvalidationInLastFrame = false;

    refresh_type eRefreshCallType = refresh_single;

    // Only redraw if there are any regions marked as "invalidated"
//    if (m_kInvalidatedRectList.IsEmpty() == false)
//    {
//        m_bInvalidationInLastFrame = true;
//        m_bCurrentlyRedrawing = true;
//#if defined _DEBUG
//        g_uiRedrawGUICounter++;
//        g_uiRedrawGUICounter %= 1000;
//#endif
//
//        if (m_kInvalidatedRectList.GetQuantity() == 1)
//        {
//            eRefreshCallType = refresh_single;
//        }
//        else
//        {
//            eRefreshCallType = refresh_first_frame;
//        }
//
//        // iterate over list of invalidated regions
//        eC_TListDoubleLinked<geo::FloatRect>::SafeIterator sIter;
//        FOR_ALL_FORWARD_SAFE (sIter, m_kInvalidatedRectList)
//        {
//            // For every invalidated region...
//            // .. set the active invalidated rectangle to this region,
//            // .. iterate over the object tree and thus redraw all affected controls
//            // .. copy updated region to visible screen
//            if( (*sIter).IsComprehensive())
//            {
//                set_invalidated_rect( *sIter);
//                clear_invalid_rect();
//                // Commence redraw.
//                start_handle_draw( m_InvalidatedRect );
//
//                // Blit refreshed part onto the screen
//                if (sIter == m_kInvalidatedRectList.GetEndSafe())
//                {
//                    if (eRefreshCallType != refresh_single)
//                    {
//                        eRefreshCallType = refresh_last_frame;
//                    }
//                }
//                refresh(eRefreshCallType);
//            }
//            eRefreshCallType = refresh_intermediate;
//        }
//    } //// if (m_kInvalidatedRectList.IsEmpty() == false )

    // Redraw completed. Now call end_redraw() callback
    end_redraw();

    // Reset invalidations
    reset_invalidation();
    m_bCurrentlyRedrawing = false;
}

void gp_wrap::invalidate_rect(const geo::FloatRect &kInvalidRect)
{
//    if( !kInvalidRect.IsComprehensive() || m_bCurrentlyRedrawing) return;
//    geo::FloatRect kNewRect( kInvalidRect);
//
//    // Merge this invalidated region into the list of other currently invalidated ones
//    eC_TListDoubleLinked<geo::FloatRect>::SafeIterator sIter;
//    FOR_ALL_FORWARD_SAFE (sIter, m_kInvalidatedRectList)
//    {
//        // If the new invalidated rect overlaps with another one in the list, unify them,
//        // and remove the former one from the list.
//        while( sIter.IsValid() && (*sIter).Overlaps( kNewRect ))
//        {
//            // Unify both rectangles
//            kNewRect.Union( (*sIter));
//            // remove old one, as this is completely covered by kNewRect now.
//            m_kInvalidatedRectList.Remove( sIter);
//            // Restart from beginning, as this may lead to further overlaps/unions
//            sIter = m_kInvalidatedRectList.GetBeginSafe();
//        }
//    }
//
//    // Finally add the newly created rectangle to the end of the list
//    m_kInvalidatedRectList.AddAtEnd( kNewRect);
}

void gp_wrap::set_screen_size(const ux_uint &uiWidth, const ux_uint &uiHeight)
{
//    GUILOG(GUI_TRACE_WARNING,
//           "CGfxWrap::set_screen_size: This method is not implemented.\n");
}

void gp_wrap::blit_img_nine_patch(const ImageResource_t &eID, const geo::FloatRect &kAbsRect, const ux_uint uiTopBorder,
                                  const ux_uint uiBottomBorder, const ux_uint uiLeftBorder, const ux_uint uiRightBorder,
                                  const ux_ubyte &alpha)
{
//    // Early return if the image is illegal
//    if( eID == DUMMY_IMAGE) return;
//
//    // Automatic Nine-Patch-Image stretching
//
//    // Initialize some values to simplify and optimize drawing code below
//    ux_uint uiImageWidth = GETGFX.get_img_width(eID);
//    ux_uint uiImageHeight = GETGFX.get_img_height(eID);
//    ux_value vDstWidth = kAbsRect.GetWidth();
//    ux_value vDstHeight = kAbsRect.GetHeight();
//
//    // Use simple blit if possible
//    if( eC_FromInt(uiImageWidth) >= vDstWidth &&
//        eC_FromInt(uiImageHeight) >= vDstHeight)
//    {
//        return blit_img_ext(eID, kAbsRect, true, alpha );
//    }
//
//    ux_value vActualLeftBorder = eC_FromInt(0);
//    ux_value vActualRightBorder = eC_FromInt(0);
//    // No need to use tiled images if the original image is larger than the destination rectangle anyway
//    if( eC_FromInt(uiImageWidth) < vDstWidth)
//    {
//        // Left Border must not be larger than the original image width
//        vActualLeftBorder = eC_Min( eC_FromInt(uiLeftBorder), eC_FromInt(uiImageWidth) );
//        // It must also not be larger than the destination image width
//        vActualLeftBorder = eC_Min( vActualLeftBorder, vDstWidth);
//        // Right Border must not be larger than the original image width
//        vActualRightBorder = eC_Min( eC_FromInt(uiRightBorder), eC_FromInt(uiImageWidth) );
//        // Left Border + Right Border must not be larger than the total image width
//        vActualRightBorder = eC_Min( vActualRightBorder, eC_FromInt(uiImageWidth)-vActualLeftBorder );
//    }
//
//    // store integer value for convenience
//    ux_uint uiActualLeftBorder = eC_ToInt(vActualLeftBorder);
//    ux_uint uiActualRightBorder = eC_ToInt(vActualRightBorder);
//
//
//    ux_value vActualTopBorder = eC_FromInt(0);
//    ux_value vActualBottomBorder = eC_FromInt(0);
//    // No need to use tiled images if the original image is larger than the destination rectangle anyway
//    if( eC_FromInt(uiImageHeight) < vDstHeight )
//    {
//        // top Border must not be larger than the original image height
//        vActualTopBorder = eC_Min( eC_FromInt(uiTopBorder), eC_FromInt(uiImageHeight) );
//        // It must also not be larger than the destination image height
//        vActualTopBorder = eC_Min( vActualTopBorder, vDstHeight);
//        // Bottom Border must not be larger than the original image height
//        vActualBottomBorder = eC_Min( eC_FromInt(uiBottomBorder), eC_FromInt(uiImageHeight));
//        // Top Border + Bottom Border must not be larger than the total image height
//        vActualBottomBorder = eC_Min( vActualBottomBorder, eC_FromInt(uiImageHeight) - vActualTopBorder);
//    }
//
//    // store integer value for convenience
//    ux_uint uiActualTopBorder = eC_ToInt(vActualTopBorder);
//    ux_uint uiActualBottomBorder = eC_ToInt(vActualBottomBorder);
//    ux_uint uiCenterSourceWidth = uiImageWidth-uiActualLeftBorder-uiActualRightBorder;
//    ux_value vCenterDstWidth = vDstWidth - vActualLeftBorder - vActualRightBorder;
//
//    if( uiActualTopBorder > 0)
//    {
//        if( uiActualLeftBorder > 0)
//        {
//            //Top Left
//            GETGFX.blit_img_ext( eID,
//                               0, 0, uiActualLeftBorder, uiActualTopBorder,
//                               kAbsRect.GetX1(), kAbsRect.GetY1(), vActualLeftBorder, vActualTopBorder, alpha);
//        }
//        if( uiActualRightBorder > 0)
//        {
//            //Top Right
//            GETGFX.blit_img_ext( eID,
//                               uiImageWidth-uiActualRightBorder, 0, uiActualRightBorder, uiActualTopBorder,
//                               kAbsRect.GetX2()-uiActualRightBorder, kAbsRect.GetY1(), vActualRightBorder, vActualTopBorder, alpha);
//        }
//        if( uiCenterSourceWidth > 0) // If source blit image width > 0
//        {
//            //Top center
//            GETGFX.blit_img_ext( eID,
//                               uiActualLeftBorder, 0, uiCenterSourceWidth, uiActualTopBorder,
//                               kAbsRect.GetX1()+vActualLeftBorder, kAbsRect.GetY1(), vCenterDstWidth, vActualTopBorder, alpha);
//        }
//    }
//
//
//    ux_uint uiMiddleSrcHeight = uiImageHeight-uiActualTopBorder-uiActualBottomBorder;
//    ux_value vMiddleDstHeight = vDstHeight- vActualTopBorder-vActualBottomBorder;
//    if( vMiddleDstHeight > 0 && uiMiddleSrcHeight > 0)
//    {
//        if( uiActualLeftBorder > 0)
//        {
//            //Middle Left
//            GETGFX.blit_img_ext( eID,
//                               0, uiActualTopBorder, uiActualLeftBorder, uiMiddleSrcHeight,
//                               kAbsRect.GetX1(), kAbsRect.GetY1()+vActualTopBorder, vActualLeftBorder, vMiddleDstHeight, alpha);
//        }
//        if( uiActualRightBorder > 0)
//        {
//            //Middle Right
//            GETGFX.blit_img_ext( eID,
//                               uiImageWidth-uiActualRightBorder, uiActualTopBorder, uiActualRightBorder, uiMiddleSrcHeight,
//                               kAbsRect.GetX2()-vActualRightBorder, kAbsRect.GetY1()+vActualTopBorder, vActualRightBorder, vMiddleDstHeight, alpha);
//        }
//        if( uiCenterSourceWidth > 0) // If source blit image width > 0
//        {
//            //Middle center
//            GETGFX.blit_img_ext( eID,
//                               uiActualLeftBorder, uiActualTopBorder, uiCenterSourceWidth, uiMiddleSrcHeight,
//                               kAbsRect.GetX1()+vActualLeftBorder, kAbsRect.GetY1()+vActualTopBorder, vCenterDstWidth, vMiddleDstHeight, alpha );
//        }
//    }
//
//    if( uiActualBottomBorder > 0)
//    {
//        ux_uint uiSrcYStart = uiImageHeight - eC_ToInt( vActualBottomBorder);
//        ux_value vDstYStart = kAbsRect.GetY2()-vActualBottomBorder;
//        if( uiActualLeftBorder > 0)
//        {
//            //Bottom Left
//            GETGFX.blit_img_ext( eID,
//                               0, uiSrcYStart, uiActualLeftBorder, uiActualBottomBorder,
//                               kAbsRect.GetX1(), vDstYStart, vActualLeftBorder, vActualBottomBorder, alpha);
//        }
//        if( uiActualRightBorder > 0)
//        {
//            //Bottom Right
//            GETGFX.blit_img_ext( eID,
//                               uiImageWidth-uiActualRightBorder, uiSrcYStart, uiActualRightBorder, uiActualBottomBorder,
//                               kAbsRect.GetX2()-vActualRightBorder,vDstYStart, vActualRightBorder, vActualBottomBorder, alpha);
//        }
//        if( uiCenterSourceWidth > 0) // If source blit image width > 0
//        {
//            //Bottom center
//            GETGFX.blit_img_ext( eID,
//                               uiActualLeftBorder, uiSrcYStart, uiCenterSourceWidth, uiActualBottomBorder,
//                               kAbsRect.GetX1()+vActualLeftBorder, vDstYStart, vCenterDstWidth, vActualBottomBorder, alpha );
//        }
//    }
}
