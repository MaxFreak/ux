//
// Created by Bernd Lappas on 29.11.15.
//

#include <cmath>

#include "gp_wrap_ogl.h"
#include "ux_types_and_defs.h"
#include "ux_exception.h"

gp_wrap_ogl::gp_wrap_ogl() : gp_wrap_gl(),
                                 m_bFBOSupported(false)
{
}

gp_wrap_ogl::~gp_wrap_ogl()
{
}

ux_bool gp_wrap_ogl::init()
{
    if(!gp_wrap_gl::init())
        return false;

    /** OpenGL initializations
    As a matter of fact,
    OGL works like a statemachine.
    Therefore features like depthbuffering, texturing and so on,
    should be be activated when needed only.
    */

    ux_string kString = (ux_char *)glGetString(GL_EXTENSIONS);
    UX_TRACE_DEBUG("CGfxWrapOGL::init: Supported extensions: "+kString+"\n");

    /* Retrieve pointer to vendor string */
    kString = (ux_char *)glGetString(GL_VENDOR);
    UX_TRACE_DEBUG("CGfxWrapOGL::init: Vendor: "+kString+"\n");

    /* Retrieve pointer to renderer string */
    kString = (ux_char *)glGetString(GL_RENDERER);
    UX_TRACE_DEBUG("CGfxWrapOGL::init: Renderer: "+kString+"\n");

    // Lighting
    glShadeModel(GL_SMOOTH); // Smooth shading calculations (anyway, does not work without light!)
    glDisable(GL_LIGHT0); // Switch off light calculation
    glDisable(GL_LIGHTING);// No material lighting desired as well

    /** Texturing
    How texels are mapped on a surface
    is described by the texture environment function (glTexEnvf).
    */
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // Only color of the texel are used
    glDisable(GL_TEXTURE_2D);

    /** Depth testing
    */
    glDisable(GL_DEPTH_TEST);// Prevent using depth buffering tests of polygons

    /** Blending
    */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha-Blending provided from back to front
    glDisable(GL_BLEND);

    /*Clipping*/

    /** Internal pixel storage modes
    */
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Read pixel
    //glPixelStorei(GL_PACK_ALIGNMENT, 1); // Write pixel

    // we only read from the back buffer because we will only draw to the back buffer
    glReadBuffer(GL_BACK);

    glDrawBuffer(GL_BACK);
    glClearColor(0.0, 0.0, 0.0, 0.0); // Sets up clear color
    glClear(GL_COLOR_BUFFER_BIT); // Clear current set Color-Buffer
    glFlush(); // Prefetch commands

    return true;
}

void gp_wrap_ogl::de_init()
{
    gp_wrap_gl::de_init();
}

ux_bool gp_wrap_ogl::Refresh(const refresh_type eRefreshCallType)
{
    glDisable(GL_SCISSOR_TEST);

    glReadBuffer(GL_BACK);
    glDrawBuffer(GL_FRONT);

    glClear(GL_DEPTH_BUFFER_BIT);// Depth Buffer

    geo::float_rect rect = get_invalidated_rect();

    ux_int iClipPosX = static_cast<ux_int>(std::floor(rect.get_x1()));
    ux_int iClipPosY = static_cast<ux_int>(std::floor((get_virtual_screen_height()) - rect.get_y2()));
    ux_int iClippingWidth = static_cast<ux_int>(std::ceil(rect.get_x2()) - std::floor(rect.get_x1()));
    ux_int iClippingHeight = static_cast<ux_int>(std::ceil(rect.get_y2()) - std::floor(rect.get_y1()));

    // All values must be not negative.
    if (iClipPosX < 0)
    {
        iClipPosX = 0;
    }

    if (iClipPosY < 0)
    {
        iClipPosY = 0;
    }

    if (iClippingWidth < 0)
    {
        iClippingWidth = 0;
    }

    if (iClippingHeight < 0)
    {
        iClippingHeight = 0;
    }

    glRasterPos2i(iClipPosX, iClipPosY);
    glCopyPixels(iClipPosX, iClipPosY, iClippingWidth, iClippingHeight, GL_COLOR);
    glFlush();

    glDrawBuffer(GL_BACK);

    glClear(GL_DEPTH_BUFFER_BIT);
    m_vDepthCorrection = 0;

    CHECK_GL_ERROR
    return true;
};

//CGUIBitmapPlane* gp_wrap_ogl::CreateGfxDependentBitmap(
//    GUIBitmapPlaneID_t uiNewBitmapID,
//    const CGUIObject& rkParentObject,
//    const ux_value &width, const ux_value &vHeight)
//{
//    if( m_bFBOSupported)
//    {
//        CGUIBitmapPlane_FBO* pNewBitmapPlane = new CGUIBitmapPlane_FBO(uiNewBitmapID, rkParentObject, width, vHeight);
//        return pNewBitmapPlane;
//    }
//    else
//    {
//        return CGfxWrapGL::CreateGfxDependentBitmap( uiNewBitmapID, rkParentObject, width, vHeight);
//    }
//}
