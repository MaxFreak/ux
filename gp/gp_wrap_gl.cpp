//
// Created by Bernd Lappas on 29.11.15.
//

#include <math.h>
#include <cmath>
#include "gp_wrap_gl.h"
#include "ux_types_and_defs.h"

#if defined UX_USE_OGL
#if !defined SYSTEM_NAME_DARWIN
// glBindBuffer and co. is defined in GUI_GL.h as CGfxWrapGL::glBindBuffer!
PFNGLBINDBUFFERPROC                     gp_wrap_gl::glBindBuffer_ux = NULL;
PFNGLBUFFERDATAPROC                     gp_wrap_gl::glBufferData_ux = NULL;
PFNGLGENBUFFERSPROC                     gp_wrap_gl::glGenBuffers_ux = NULL;
PFNGLDELETEBUFFERSPROC                  gp_wrap_gl::glDeleteBuffers_ux = NULL;
PFNGLGENFRAMEBUFFERSPROC                gp_wrap_gl::glGenFramebuffers_ux = NULL;
PFNGLBINDFRAMEBUFFERPROC                gp_wrap_gl::glBindFramebuffer_ux = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC           gp_wrap_gl::glFramebufferTexture2D_ux = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC         gp_wrap_gl::glCheckFramebufferStatus_ux = NULL;
PFNGLDELETEFRAMEBUFFERSPROC             gp_wrap_gl::glDeleteFramebuffers_ux = NULL;
#endif
#endif

// Helper macro for coordinate conversion to OpenGL
// For Y conversion, we need to flip the Y value
// (because the origin in OpenGL is in the bottom left corner, i.e. in the first quadrant,
// whereas ux's origin is in the top left corner, i.e. in the second quadrant)
#define TRANS_Y_COORD(a) (static_cast<ux_float>(get_virtual_screen_height()) - (a))

const ux_value gp_wrap_gl::cvPixelCenterOffset = 0.375;

gp_wrap_gl::gp_wrap_gl() : gp_wrap(),
                           m_uiScreenWidth(0), m_uiScreenHeight(0), m_vRed(0), m_vGreen(0), m_vBlue(0),
                           m_vAlpha(0), m_vDepthCorrection(0), m_vFovy(0), m_vAspect(0), m_vNear(0), m_vFar(0),
                           m_uiVBO(0), m_uiUVVBO(0)
#if defined UX_USE_OGLES2
    ,m_uiVertexShaderHandle(0),
    m_uiFragmentShaderHandle(0),
    m_uiProgramObject(0),
    m_eActiveMatrix(gp_wrap_gl::MT_PROJECTION)
#endif
{
#if defined UX_USE_OGLES2
    memset(m_avProjectionMatrix, 0, sizeof(Matrix3D_t));
    memset(m_avModelviewMatrix, 0, sizeof(Matrix3D_t));
#endif
    m_apTextures.clear();
    init();
}

gp_wrap_gl::~gp_wrap_gl()
{
    de_init();

#if defined GUILIANI_USE_OGLES2
    // Frees the OpenGL handles for the program and the 2 shaders
    glDeleteProgram(m_uiProgramObject);
    glDeleteShader(m_uiFragmentShaderHandle);
    glDeleteShader(m_uiVertexShaderHandle);
#endif

    // Delete the VBO as it is no longer needed
    union
    {
        // Due to strict-aliasing rule of the C99 standard, we use this
        // workaround for converting a ux_uint* to GLuint*.
        GLuint *pglui;
        ux_uint *pui;
    } peui;
    if (m_uiVBO != 0)
    {
        peui.pui = &m_uiVBO;
        glDeleteBuffers(1, peui.pglui);
    }
    if (m_uiUVVBO != 0)
    {
        peui.pui = &m_uiUVVBO;
        glDeleteBuffers(1, peui.pglui);
    }
    m_apTextures.clear();
}

ux_bool gp_wrap_gl::init()
{
    if(!gp_wrap::init())
        return false;

    /** Most newer OpenGL cards (on PC) enforce flipping of back and front buffers.
        Therefore we initialize set_doublebuffering_enforces_flipping with TRUE by default */
    set_doublebuffering_enforces_flipping(true);

    try
    {
        m_apTextures.resize(m_nof_images);
        for (ux_uint i = 0; i < m_nof_images; i++)
        {
            m_apTextures[i] = NULL;
        }
    }
    catch (...)
    {
        UX_TRACE_ERROR("CGfxWrapOGL::init: Could not initialize 'image buffer'.\n");
        return false;
    }

    return true;
}

void gp_wrap_gl::de_init()
{
    // Delete all existing textures
    ux_uint ui;
    for (ui = 0; ui < m_nof_images; ui++)
    {
//        unload_img(UNMAP_IMGENTRY_TO_ARRAY((image_id)ui));
    }
}

void gp_wrap_gl::set_2D_view()
{
    set_2D_view(get_virtual_screen_width(), get_virtual_screen_height());
}

void gp_wrap_gl::set_2D_view(const ux_uint width, const ux_uint height)
{
    // Clear projection matrix for adjusting view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set orthogonal 2D perspective,
    // this sets up also the default clipping plane of the projection
    glOrtho(0, width, // Left, Right
            0, height,     // Bottom, Top
            1, -1);       // Near, Far

    // Marks the drawing region
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void gp_wrap_gl::create_screen(const ux_uint &width, const ux_uint &height)
{
    // Window information is stored in OGL-Base class
    // because of the need to recalculate into the different coordinate system of Guiliani
    m_uiScreenWidth = width;
    m_uiScreenHeight = height;

    set_2D_view();
}

void gp_wrap_gl::set_foreground_color_impl(const ux_ubyte &red, const ux_ubyte &green, const ux_ubyte &blue,
                                           const ux_ubyte &alpha)
{
    m_vRed = 0;
    m_vGreen = 0;
    m_vBlue = 0;
    m_vAlpha = 0;

    // Only enable blending if necessary
    if (alpha == 255)
    {
        glDisable(GL_BLEND);
    }
    else
    {
        glEnable(GL_BLEND);
    }

    if (red != 0)
    {
        m_vRed = red / 255.;
    }
    if (green != 0)
    {
        m_vGreen = green / 255.;
    }
    if (blue != 0)
    {
        m_vBlue = blue / 255.;
    }
    if (alpha != 0)
    {
        m_vAlpha = alpha / 255.;
    }

    glColor4f(m_vRed, m_vGreen, m_vBlue, m_vAlpha);

    m_color = UINT_FROM_ARGB(alpha, red, green, blue);
    CHECK_GL_ERROR
}

ux_value gp_wrap_gl::set_line_width(const ux_value &width)
{
    glLineWidth(width);
    return gp_wrap_gl::set_line_width(width);
}

void gp_wrap_gl::line(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2)
{
    // do not use texturing
#if defined UX_USE_OGLES2
    glUniform1i(glGetUniformLocation(m_uiProgramObject, "TextureEnvMode"), 0);
#else
    glDisable(GL_TEXTURE_2D);
#endif
    glDisableClientState(GL_COLOR_ARRAY); // Disable per vertex color
    glEnableClientState(GL_VERTEX_ARRAY);

    // Note: OpenGL Pixel Coordinates do not refer to the "center" of the pixel, but to the edges between them.
    // This means 0/0 would not point to the center of the top/left pixel, but instead to the top/left corner of this pixel
    // Therefore we move the coordinates to the center of the pixel by adding 0.375 pixels.

    ux_value avVertices[] = {
        vX1 + cvPixelCenterOffset, TRANS_Y_COORD(vY1 + cvPixelCenterOffset),
        vX2 + cvPixelCenterOffset, TRANS_Y_COORD(vY2 + cvPixelCenterOffset)};

    glBindBuffer(GL_ARRAY_BUFFER, get_VBO());
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(avVertices), avVertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, 2);

    CHECK_GL_ERROR
}

void gp_wrap_gl::draw_rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2,
                           const ux_bool filled)
{
    // do not use texturing
#if defined UX_USE_OGLES2
    glUniform1i(glGetUniformLocation(m_uiProgramObject, "TextureEnvMode"), 0);
#else
    glDisable(GL_TEXTURE_2D);
#endif

    glDisableClientState(GL_COLOR_ARRAY); // Disable per vertex color
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, get_VBO());
    glVertexPointer(2, GL_FLOAT, 0, 0);

    if (filled)
    {
        ux_value avVertices[] = {
            vX1 + cvPixelCenterOffset, TRANS_Y_COORD(vY1 + cvPixelCenterOffset),   // Top left corner
            vX1 + cvPixelCenterOffset, TRANS_Y_COORD(vY2 + cvPixelCenterOffset),   // Bottom left corner
            vX2 + cvPixelCenterOffset, TRANS_Y_COORD(vY2 + cvPixelCenterOffset),   // Bottom right corner
            vX2 + cvPixelCenterOffset, TRANS_Y_COORD(vY1 + cvPixelCenterOffset)};  // Top right corner
        glBufferData(GL_ARRAY_BUFFER, sizeof(avVertices), avVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    else
    {
        // Yes, the following code block is almost a duplicate of the one above, but since all of the method's parameters are const
        // we would have to use const_cast or temporary variables if we tried to write this code only once.
        ux_value avVertices[] = {
            vX1 + cvPixelCenterOffset, TRANS_Y_COORD(vY1 + cvPixelCenterOffset),                 // Top left corner
            vX1 + cvPixelCenterOffset, TRANS_Y_COORD(vY2 + cvPixelCenterOffset - 1),   // Bottom left corner
            vX2 + cvPixelCenterOffset - 1, TRANS_Y_COORD(vY2 + cvPixelCenterOffset - 1),   // Bottom right corner
            vX2 + cvPixelCenterOffset - 1, TRANS_Y_COORD(vY1 + cvPixelCenterOffset)};                // Top right corner
        glBufferData(GL_ARRAY_BUFFER, sizeof(avVertices), avVertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

/*
    // Note: OpenGL Pixel Coordinates do not refer to the "center" of the pixel, but to the edges between them.
    // This means 0/0 would not point to the center of the top/left pixel, but instead to the top/left corner of this pixel
    // Therefore we move the coordinates to the center of the pixel by adding 0.375 pixels.
    ux_value vTempX1 = vX1+ cvPixelCenterOffset;
    ux_value vTempY1 = TRANS_Y_COORD(vY1+cvPixelCenterOffset);
    ux_value vTempX2 = vX2+ cvPixelCenterOffset;
    ux_value vTempY2 = TRANS_Y_COORD(vY2+cvPixelCenterOffset);

    GLenum  eDrawMode = GL_TRIANGLE_FAN;
    if(filled)
    {
        // Surprisingly we explicitly need to exclude the lower right column/row of the rect for line-loops but not for triangle-fans
        eDrawMode = GL_TRIANGLE_FAN;
    }
    else
    {
        // Surprisingly we explicitly need to exclude the lower right column/row of the rect for line-loops but not for triangle-fans
        vTempX2 -= eC_FromInt(1);
        vTempY2 += eC_FromInt(1);
        eDrawMode = GL_LINE_LOOP;
    }

    ux_value avVertices[] = {
        vTempX1, vTempY1,   // Top left corner
        vTempX1, vTempY2,   // Bottom left corner
        vTempX2, vTempY2,   // Bottom right corner
        vTempX2, vTempY1};  // Top right corner

    glBufferData(GL_ARRAY_BUFFER, sizeof(avVertices), avVertices, GL_DYNAMIC_DRAW);
    glDrawArrays(eDrawMode, 0, 4);
*/
    CHECK_GL_ERROR
}

void gp_wrap_gl::rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2)
{
    draw_rect(vX1, vY1, vX2, vY2, false);
}

void gp_wrap_gl::filled_rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2)
{
    draw_rect(vX1, vY1, vX2, vY2, true);
}

void gp_wrap_gl::ellipse(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2,
                         const ux_bool &filled)
{
    // do not use texturing
#if defined GUILIANI_USE_OGLES2
    glUniform1i(glGetUniformLocation(m_uiProgramObject, "TextureEnvMode"), 0);
#else
    glDisable(GL_TEXTURE_2D);
#endif
    glDisableClientState(GL_COLOR_ARRAY); // Disable per vertex color
    glEnableClientState(GL_VERTEX_ARRAY);

    const ux_value cvTwoPi = 6.283185307179586476925286766559f;

    const ux_uint cuiSteps = 30; /// @todo Mapping function depending on ellipse size
    ux_value width = vX2 - vX1;
    ux_value height = vY2 - vY1;
    ux_value vWidthHalf = width / 2;
    ux_value vHeightHalf = height / 2;
    ux_value vCenterX = vX1 + vWidthHalf;
    ux_value vCenterY = TRANS_Y_COORD(vY1 + vHeightHalf);
    ux_value vStepSize = cvTwoPi / cuiSteps;
    ux_value avVertices[(cuiSteps + 1) * 2];
    ux_int i = 0;
    for (ux_value s = 0; s < cvTwoPi; s += vStepSize)
    {
        avVertices[i++] = vWidthHalf * cos(s) + vCenterX;
        avVertices[i++] = vHeightHalf * sin(s) + vCenterY;
    }
    glBindBuffer(GL_ARRAY_BUFFER, get_VBO());
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(avVertices), avVertices, GL_DYNAMIC_DRAW);

    if (filled)
    {
        glDrawArrays(GL_TRIANGLE_FAN, 0, cuiSteps);
    }
    else
    {
        glDrawArrays(GL_LINE_LOOP, 0, cuiSteps);
    }
}

void gp_wrap_gl::load_img_impl(const ux_string &kPath, const image_id &eImageID)
{
//    ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(eImageID);
//
//    // try to load the image
//    try
//    {
////        GUILOG(GUI_TRACE_DEBUG,"CGfxWrapGL::load_img_impl: Loading...\n");
//        m_apTextures[uiCurImage] = new CGUIImageDataGL();
//        m_apTextures[uiCurImage]->SetImageType(CGUIImageData::RGBA_8888);
//        m_apTextures[uiCurImage]->SetStartTopLeft(false);
//
//        // Loads an image to the current bound ID
//        if(!GETIMGLOADER.load_img(*m_apTextures[uiCurImage], kPath)) // could not load image from file pointer
//        {
//            GUILOG(GUI_TRACE_ERROR, "CGfxWrapGL::load_img_impl: Problem loading the image from file:\nFilename: '" + kPath + "'.\n");
//        }
//    }
//    catch (...)
//    {
//        delete m_apTextures[uiCurImage];
//        m_apTextures[uiCurImage] = NULL;
//        GUILOG(GUI_TRACE_ERROR, "CGfxWrapGL::load_img_impl: Problem while loading image.\n");
//        return;
//    }
}

void gp_wrap_gl::unload_img(const image_id &eImageID)
{
//    // Get image ID
//    ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(eImageID);
//
//    // return if there is no matching image for the given image ID
//    if (uiCurImage >= m_nof_images || m_apTextures[uiCurImage] == NULL)
//        return;
//
//    // Delete existing image
//    //Access the array directly instead of calling get_texture_ID to avoid generation of the texture.
//    const GLuint cuiTextureID = (GLuint)m_apTextures[uiCurImage]->get_texture_ID();
//    if( cuiTextureID != 0)
//    {
//        glDeleteTextures(1, &cuiTextureID);
//    }
//
//    delete m_apTextures[uiCurImage];
//    m_apTextures[uiCurImage] = NULL;
}

ux_uint gp_wrap_gl::get_img_width_impl(const image_id &eImageID) const
{
    //Map ImageIDs to 0-m_nof_images
//    ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(eImageID);
//    return m_apTextures[uiCurImage]->GetWidth();
    return 0;
}

ux_uint gp_wrap_gl::get_img_height_impl(const image_id &eImageID) const
{
//    //Map ImageIDs to 0-m_nof_images
//    ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(eImageID);
//    return m_apTextures[uiCurImage]->GetHeight();
    return 0;
}

void gp_wrap_gl::blit_img_ext_impl(const image_id &eImageID, const ux_uint &uiSrcX, const ux_uint &uiSrcY,
                                   const ux_uint &src_width, const ux_uint &src_height, const ux_value &vDestX,
                                   const ux_value &vDestY, const ux_value &dest_width, const ux_value &dest_height,
                                   const ux_ubyte &alpha, const ux_value &angle, const ux_value &rot_center_x,
                                   const ux_value &rot_center_y)
{
//    // Retrieve current cliprect
//    geo::float_rect kClipRect;
//    get_cliprect( kClipRect);
//
//    // Early return if blit occurs outside of the active clipping area
//    // The blit would be clipped away anyway, but we can save the effort for setting GL-states
//    // This will also early-return if e.g. illegal width/height values are supplied
//    if( !geo::float_rect( vDestX, vDestY, vDestX+dest_width, vDestY+dest_height).Overlaps( kClipRect ))
//        return;
//
//    //Map ImageIDs to array index (starting with 0)
//    ux_uint uiCurImage =MAP_IMGID_TO_ARRAY(eImageID);
//
//    // If a texture for this image exists
//    if(get_texture_ID(eImageID) != 0)
//    {
//        /// @todo Positions of textures are a little bit wrong in OpenGL-ES.
//        //Maybe a rounding error causes the use of fixpoint?
//        ux_value vTexWidth = eC_FromInt(m_apTextures[uiCurImage]->GetTextureWidth());
//        ux_value vTexHeight = eC_FromInt(m_apTextures[uiCurImage]->GetTextureHeight());
//
//        if (vTexWidth != 0 && vTexHeight != 0)
//        {
//            // Set alpha blending
//            glEnable(GL_BLEND); // Enable alpha blending
//            ux_value vAlpha = eC_FromInt(0);
//            if(alpha != 0)
//            {
//                vAlpha = eC_FromFloat(alpha/255.0f);
//            }
//            glColor4V(eC_FromInt(1), eC_FromInt(1), eC_FromInt(1), vAlpha);
//
//            // bind buffer
//            glBindBuffer(GL_ARRAY_BUFFER, get_VBO());
//
//            glDisableClientState(GL_COLOR_ARRAY); // Disable per vertex color
//            glEnableClientState(GL_VERTEX_ARRAY);
//            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//
//            glBindTexture(GL_TEXTURE_2D, get_texture_ID(eImageID));
//            ux_uint uiFlippedSrcY = m_apTextures[uiCurImage]->GetHeight() - (uiSrcY + src_height);
//
//#if defined GUILIANI_USE_OGLES2
//            // Sets the fragment texture env mode to GL_MODULATE
//            glUniform1i(glGetUniformLocation(m_uiProgramObject, "TextureEnvMode"), 1);  // Blend texel with color fragment
//#else
//            glEnable(GL_TEXTURE_2D);
//            glTexEnvV(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Blend texel with color fragment
//#endif
//            ux_value avVertices[] = {
//                vDestX, TRANS_Y_COORD(vDestY + dest_height), //0,0
//                eC_Div( eC_FromInt(uiSrcX), vTexWidth), //U
//                eC_Div( eC_FromInt(uiFlippedSrcY), vTexHeight), //V
//
//                vDestX + dest_width, TRANS_Y_COORD(vDestY + dest_height), //1,0
//                eC_Div( eC_FromInt(uiSrcX + src_width), vTexWidth ), //U
//                eC_Div( eC_FromInt(uiFlippedSrcY), vTexHeight), //V
//
//                vDestX, TRANS_Y_COORD(vDestY), //0,1
//                eC_Div( eC_FromInt(uiSrcX), vTexWidth ), //U
//                eC_Div( eC_FromInt(uiFlippedSrcY + src_height), vTexHeight), //V
//
//                vDestX + dest_width, TRANS_Y_COORD(vDestY), //1,1
//                eC_Div( eC_FromInt(uiSrcX + src_width), vTexWidth ), //U
//                eC_Div( eC_FromInt(uiFlippedSrcY + src_height), vTexHeight ) //V
//            };
//
//            // Set the buffer's data
//            ux_uint uiVertexStride = (2 * sizeof(ux_value))*2; // Calc avVertices size (2 eC_Values for the pos, 2 for the UVs))
//            glBufferData(GL_ARRAY_BUFFER, sizeof(avVertices), avVertices, GL_DYNAMIC_DRAW);
//
//            // Pass the vertex data
//            glVertexPointer(2, GL_EC_VALUE, uiVertexStride, 0);
//
//            // Pass the texture coordinates data
//            glTexCoordPointer(2, GL_EC_VALUE, uiVertexStride, (void*) (2 * sizeof(ux_value)));
//
//            // If there is a rotation requested for this blit, we need to apply several transformations to the image
//            if( angle != 0)
//            {
//                glPushMatrix();
//                // Translate to actual screen position
//                // Remember that matrix operations affect the result in reverse order!)
//                // So effectively we move to the origin first, then rotate and finally move to the target position
//                glTranslateV( vDestX + eC_Mul(dest_width,rot_center_x), TRANS_Y_COORD(vDestY+eC_Mul(dest_height,rot_center_y)), 0);
//                // Rotate around Z axis
//                glRotateV( -angle, 0,0, eC_FromInt(1));
//                // Move image towards opengl-origin for rotation around rot_center_x/Y
//                glTranslateV( -(vDestX + eC_Mul(dest_width,rot_center_x)), -(TRANS_Y_COORD(vDestY+eC_Mul(dest_height,rot_center_y))), 0);
//            }
//
//            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//            // Restore original matrix, if we applied a rotation to the current one
//            if( angle != 0)
//            {
//                glPopMatrix();
//            }
//        }
//    }
//    else // No image was bound to specific ID!
//    {
//        GUILOG(GUI_TRACE_ERROR, "CGfxWrapOGL::blit_img_ext: No image bound at ID: '"
//            + ux_string(MAP_IMGID_TO_ARRAY(eImageID)) + "'\n");
//    }
}

ux_uint gp_wrap_gl::get_image_size(const image_id &id) const
{
//    ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(id);
//    if( uiCurImage >= m_nof_images)
//    {
//        GUILOG(GUI_TRACE_ERROR, "CGfxWrapGL::get_image_size: Illegal image ID (" + ux_string(id) + ") supplied.\n");
//        return 0;
//    }
//
//    if( !image_exists( id) )
//    {
//        return 0;
//    }
//
//    ux_uint uiSize =0;
//    // Calculate memory required by image.
//    // The assumptions here are:
//    // 1.) Images in Guiliani OpenGL wrappers will always have 4 Byte per pixel
//    // 2.) We take the "real" image size, not any potentially corrected size with power of 2 dimensions
//    uiSize = m_apTextures[uiCurImage]->GetWidth() * m_apTextures[uiCurImage]->GetHeight() * 4;
//
//    return uiSize;
    return 0;
}

void gp_wrap_gl::set_cliprect(const geo::float_rect &rkNewClipRect)
{
    set_cliprect(rkNewClipRect, false);
}

void gp_wrap_gl::set_cliprect(const geo::float_rect &rkNewClipRect, const ux_bool &bRotate)
{
    gp_wrap_gl::set_cliprect(rkNewClipRect);
    geo::float_rect tempRect(m_clipping_rect);

    if (true == bRotate)
    {
        // Rotate the the clip rect
        geo::float_rect kRotRect(rkNewClipRect.get_y1(), get_physical_screen_height() - rkNewClipRect.get_x2(),
                                 rkNewClipRect.get_y2(), get_physical_screen_height() - rkNewClipRect.get_x1());
        tempRect = kRotRect;
    }

    ux_int iClipPosX = 0;
    ux_int iClipPosY = 0;
    ux_int iClippingWidth = 0;
    ux_int iClippingHeight = 0;

//    if (tempRect.IsComprehensive())
    {
        // Prevent negative values
        if (tempRect.get_x1() < 0)
            tempRect.m_Left = 0;
        if (tempRect.get_x2() < 0)
            tempRect.m_Width = 0;
        if (tempRect.get_y1() < 0)
            tempRect.m_Top = 0;
        if (tempRect.get_y2() < 0)
            tempRect.m_Height = 0;

        iClipPosX = static_cast<ux_int>(std::floor(tempRect.get_x1()));
        iClipPosY = static_cast<ux_int>(std::floor(TRANS_Y_COORD(tempRect.get_y2())));

        iClippingWidth = static_cast<ux_int>(std::ceil(tempRect.get_x2()) - std::floor(tempRect.get_x1()));
        iClippingHeight = static_cast<ux_int>(std::ceil(tempRect.get_y2()) - std::floor(tempRect.get_y1()));
    }
//    else
//    {
//        iClipPosX = 0;
//        iClipPosY = 0;
//        iClippingWidth = 0;
//        iClippingHeight = 0;
//    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(iClipPosX, iClipPosY, iClippingWidth, iClippingHeight);
}

ux_bool gp_wrap_gl::check_GL_error(const ux_string &kCallingMethod)
{
    ux_bool bError = false;
    GLenum err = 0;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        bError = true;
        UX_TRACE_ERROR(kCallingMethod + ": OpenGL Error " + std::to_string(err) + "\n");
    };
    return bError;
}

ux_bool gp_wrap_gl::set_nof_imagesImpl(const ux_uint uiNOFImages)
{
    if (uiNOFImages > m_nof_images)
    {
        ux_uint uiNOFNewImages = uiNOFImages - m_nof_images;
        m_nof_images = uiNOFImages;

        // Resize internal array of images
        // ATTENTION: Resizing the Array only works with simple data types
        // such as pointers or the basic data types (int, float, char... )
//        m_apTextures.SetQuantity(uiNOFImages, true);
        m_apTextures.resize(uiNOFImages);

        // initialize newly created images
        for (ux_uint ui = 1; ui <= uiNOFNewImages; ui++)
        {
            m_apTextures[uiNOFImages - ui] = NULL;
        }
        return true;
    }
    return false;
}

//ux_image_data_gl* gp_wrap_gl::GetImageData(const image_id &eImageID)
//{
//    if (eImageID != DUMMY_IMAGE)
//    {
//        //Map ImageIDs to array index (starting with 0)
//        ux_uint uiCurImage =MAP_IMGID_TO_ARRAY(eImageID);//=image_id(eImageID-DUMMY_IMAGE);
//        if (uiCurImage >= m_nof_images || m_apTextures[uiCurImage] == NULL)
//        {
//            return NULL;
//        }
//
//        //Texture must be generated if it is not done before.
//        //So get_texture_ID is called here, this method does this for us.
//        get_texture_ID(eImageID);
//
//        // Get image ID
//        return m_apTextures[uiCurImage];
//    }
//    return NULL;
//}

//void gp_wrap_gl::generate_texture(const image_id &eImageID)
//{
//    ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(eImageID);
//
//    // Start process generating texture from image data
//    m_apTextures[uiCurImage]->SetTextureWidth(get_next_power_of_2(eC_FromInt(m_apTextures[uiCurImage]->GetWidth())));
//
//    // If width and height are equal
//    if(m_apTextures[uiCurImage]->GetHeight() == m_apTextures[uiCurImage]->GetWidth())
//    {
//        m_apTextures[uiCurImage]->SetTextureHeight(m_apTextures[uiCurImage]->GetTextureWidth());
//    }
//        // Detect next power of 2 height dependent on the image height
//    else
//    {
//        m_apTextures[uiCurImage]->SetTextureHeight(get_next_power_of_2(eC_FromInt(m_apTextures[uiCurImage]->GetHeight())));
//    }
//
//    // Get matching buffer for generating a power of 2 texture
//    ux_uint uiTotalTextureSize = m_apTextures[uiCurImage]->GetTextureWidth() * m_apTextures[uiCurImage]->GetTextureHeight() * m_apTextures[uiCurImage]->GetBytePerPixel();
//    CGUIAutoPtr<ux_ubyte> pubTexData(new ux_ubyte[uiTotalTextureSize], true);
//    memset(pubTexData.get(), 0x00, uiTotalTextureSize);
//
//    if (m_apTextures[uiCurImage]->GetData())
//    {
//        // Fill the texture buffer with actual image data
//        for(unsigned int uiSrcRow = 0; uiSrcRow < m_apTextures[uiCurImage]->GetHeight(); uiSrcRow++)
//        {
//            // Row orientated
//            memcpy(&pubTexData.get()[uiSrcRow * m_apTextures[uiCurImage]->GetTextureWidth() * m_apTextures[uiCurImage]->GetBytePerPixel()],
//                   &m_apTextures[uiCurImage]->GetData()[uiSrcRow * m_apTextures[uiCurImage]->GetWidth() * m_apTextures[uiCurImage]->GetBytePerPixel()],
//                   m_apTextures[uiCurImage]->GetWidth() * m_apTextures[uiCurImage]->GetBytePerPixel());
//        }
//    }
//
//    GLuint uiID;
//    glGenTextures(1, &uiID); // Get an free texture ID
//    glBindTexture(GL_TEXTURE_2D, uiID);
//    m_apTextures[uiCurImage]->SetID(uiID); // store texture ID
//
//    // Behaviour while texture is mapped onto an object surface
//    glTexParameterV(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Texture > Object
//    glTexParameterV(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture < Object
//
//    glTexParameterV(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterV(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
//                 m_apTextures[uiCurImage]->GetTextureWidth(),
//                 m_apTextures[uiCurImage]->GetTextureHeight(),
//                 0, GL_RGBA, GL_UNSIGNED_BYTE, pubTexData.get());
//
//    // image data is not needed anymore only the dimension infos and the id
//    m_apTextures[uiCurImage]->DeleteData();
//}

//CGUIBitmapPlane* gp_wrap_gl::CreateGfxDependentBitmap(
//    GUIBitmapPlaneID_t uiNewBitmapID,
//    const CGUIObject& rkParentObject,
//    const ux_value &width, const ux_value &height)
//{
//    CGUIBitmapPlane_GL* pNewBitmapPlane = new CGUIBitmapPlane_GL(uiNewBitmapID, rkParentObject, width, height);
//    pNewBitmapPlane->CreateRenderSurface();
//
//    return pNewBitmapPlane;
//}

inline ux_uint gp_wrap_gl::get_virtual_screen_width()
{
    ux_uint uiScreenWidth = m_uiScreenWidth;
//    if((GetUsedBitmapPlaneID() != 0) && (GetBitmapPlane(GetUsedBitmapPlaneID()).GetID() != 0))
//    {
//        uiScreenWidth = eC_ToInt(eC_Ceil(GetBitmapPlane(GetUsedBitmapPlaneID()).GetWidth()));
//    }
    return uiScreenWidth;
}

inline ux_uint gp_wrap_gl::get_physical_screen_width()
{
    return m_uiScreenWidth;
}

inline ux_uint gp_wrap_gl::get_physical_screen_height()
{
    return m_uiScreenHeight;
}

inline ux_uint gp_wrap_gl::get_virtual_screen_height()
{
    ux_uint uiScreenHeight = m_uiScreenHeight;
//    if((GetUsedBitmapPlaneID() != 0) && (GetBitmapPlane(GetUsedBitmapPlaneID()).GetID() != 0))
//    {
//        uiScreenHeight = eC_ToInt(eC_Ceil(GetBitmapPlane(GetUsedBitmapPlaneID()).GetHeight()));
//    }
    return uiScreenHeight;
}

const ux_uint gp_wrap_gl::get_texture_ID(const image_id &eImageID)
{

    return 0;
}

void gp_wrap_gl::set_perspective_projection(const ux_value &vFovy, const ux_value &vAspect, const ux_value &vNear,
                                            const ux_value &vFar)
{
    // if none of the parameters has changed set old matrix and return
    if (vFovy == m_vFovy && vAspect == m_vAspect && vNear == m_vNear && vFar == m_vFar)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf((GLfloat *) (m_avTransMat));
        return;
    }

    // clear matrix
    memset(m_avTransMat, 0, sizeof(Matrix3D_t));

    const ux_value cvPi = 3.141592654;
    // recalculate values
    const ux_value cvDeg2Rad = cvPi / 180.f;
    ux_value vH = std::tan(vFovy / 2.f * cvDeg2Rad);
    ux_value vW = vAspect * vH;
    ux_value vD = vFar - vNear;

    // set values
    m_avTransMat[0][0] = 1.f / vW;
    m_avTransMat[1][1] = 1.f / vH;
    m_avTransMat[2][2] = -1.f * (vNear + vFar) / vD;
    m_avTransMat[2][3] = -1.f;
    m_avTransMat[3][2] = -2.f * (vNear * (vFar / vD));

    //keep values
    m_vFovy = vFovy;
    m_vAspect = vAspect;
    m_vNear = vNear;
    m_vFar = vFar;

    //set matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((GLfloat *) (m_avTransMat));
}

ux_bool gp_wrap_gl::init_view()
{
#if defined GUILIANI_USE_OGLES2
    InitShaders();
#endif

    union
    {
        // Due to strict-aliasing rule of the C99 standard, we use this
        // workaround for converting a GLuint* to ux_uint*.
        GLuint *pglui;
        ux_uint *pui;
    } peui;
    // Generate the vertex buffer object (VBO)
    peui.pui = &m_uiVBO;
    glGenBuffers(1, peui.pglui);

    // Generate the UV vertex buffer object (VBO)
    peui.pui = &m_uiUVVBO;
    glGenBuffers(1, peui.pglui);
    return true;
}


ux_value gp_wrap_gl::get_z_correction()
{
//    if(GetUsedBitmapPlaneID() != 0)
//    {
//        // we can be sure that only CGUIBitmapPlane_GL or inherited BitmapPlanes are used
//        return static_cast<CGUIBitmapPlane_GL&>(GetBitmapPlane(GetUsedBitmapPlaneID())).m_vDepthCorrection;
//    }
//    else
    {
        return m_vDepthCorrection;
    }
}

void gp_wrap_gl::increase_z_correction()
{
//    if(GetUsedBitmapPlaneID() != 0)
//    {
//        // we can be sure that only CGUIBitmapPlane_GL or inherited BitmapPlanes are used
//        static_cast<CGUIBitmapPlane_GL&>(GetBitmapPlane(GetUsedBitmapPlaneID())).m_vDepthCorrection += eC_FromFloat(1);
//    }
//    else
    {
        m_vDepthCorrection += 1.f;
    }
}

void gp_wrap_gl::set_screen_size(const ux_uint &width, const ux_uint &height)
{
    m_uiScreenWidth = width;
    m_uiScreenHeight = height;
    set_2D_view();
}

void gp_wrap_gl::restore_back_buffer(const ux_value width, const ux_value height)
{
    // Make sure view frustum is set correctly
    set_2D_view();

    //store the real clip and invalidated rect
//    geo::float_rect kRealInvalidatedRect = GETGFX.get_invalidated_rect();
//    geo::float_rect kRealClipRect;
//    GETGFX.get_cliprect(kRealClipRect);
    geo::float_rect kRealInvalidatedRect = get_invalidated_rect();
    geo::float_rect kRealClipRect;
    get_cliprect(kRealClipRect);

    geo::float_rect kClipRect(0.f, TRANS_Y_COORD(height), width, m_uiScreenHeight);

    set_invalidated_rect(kClipRect);
    set_cliprect(kClipRect);
    start_handle_draw(kClipRect);

    // restore the real clip and invalidated rect
    set_invalidated_rect(kRealInvalidatedRect);
    set_cliprect(kRealClipRect);
}


#if defined UX_USE_OGLES2
void gp_wrap_gl::InitShaders()
{
    GLint iShaderCompiled = 0;

    // Fragment and vertex shaders code
    const ux_char* cFragmentShaderSource = "\
        uniform sampler2D TexSampler;          /* Texture sampler */ \
        varying lowp vec4 FragmentShaderColor; /* The color set by Color variable in vertex shader */ \
        varying mediump vec4 TexCoords;        /* The texture coords set by UV variable in vertex shader */ \
        uniform int TextureEnvMode;            /* The texture environment mode */ \
        void main() \
        { \
            if (TextureEnvMode == 1) /* Does the GL_MODULATE (calculate texture color with set color) */ \
            { \
                lowp vec4 TexColor = texture2D(TexSampler,TexCoords.st); \
                gl_FragColor.r = FragmentShaderColor.r * TexColor.r; \
                gl_FragColor.g = FragmentShaderColor.g * TexColor.g; \
                gl_FragColor.b = FragmentShaderColor.b * TexColor.b; \
                gl_FragColor.a = FragmentShaderColor.a * TexColor.a; \
            } \
            else /* Sets the color directly without using texture color (must be used while drawing vertices without texture). Similar to glDisable(GL_TEXTURE)*/ \
            { \
                gl_FragColor = FragmentShaderColor; \
            } \
        }";

    const ux_char* cVertexShaderSource = "\
        attribute vec4 Vertex;              /* Use this variable to pass the vertices */ \
        attribute vec4 UV;                  /* Use this variable to pass the UV tex coords */ \
        varying vec4 TexCoords;             /* This variable shares the TexCoords with fragment shader */ \
        uniform vec4 Color;                 /* This variable sets the color */ \
        varying vec4 FragmentShaderColor;   /* This variable shares the color with the fragment shader (keyword: varying!) */ \
        uniform mat4 ProjectionMatrix;      /* Use this uniform variable to pass the projection matrix */ \
        uniform mat4 ModelviewMatrix;       /* Use this uniform variable to pass the modelview matrix */ \
        uniform int TextureEnvMode;         /* Sets the texture enviroment mode */ \
        void main()\
        {\
            gl_Position = ProjectionMatrix * ModelviewMatrix * Vertex;                          /* Calculate the position */ \
            FragmentShaderColor = Color;                                                        /* Set fragment shader color */ \
            TexCoords = UV;                                                                     /* Set texture coords. */ \
        }";

    // Create the fragment shader object
    m_uiFragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

    // Load the source code into it
    glShaderSource(m_uiFragmentShaderHandle, 1, static_cast<const char**>(&cFragmentShaderSource), NULL);

    // Compile the source code
    glCompileShader(m_uiFragmentShaderHandle);

    // Check if compilation succeeded
    glGetShaderiv(m_uiFragmentShaderHandle, GL_COMPILE_STATUS, &iShaderCompiled);

    if (false == iShaderCompiled)
    {
        // An error happened, first retrieve the length of the log message
        GLint iInfoLogLength;
        GLsizei iCharsWritten;
        glGetShaderiv(m_uiFragmentShaderHandle, GL_INFO_LOG_LENGTH, &iInfoLogLength);

        // Allocate enough space for the message and retrieve it
        CGUIAutoPtr<ux_char> pcInfoLog(new ux_char[iInfoLogLength], true);
        glGetShaderInfoLog(m_uiFragmentShaderHandle, iInfoLogLength, &iCharsWritten, pcInfoLog.get());

        GUILOG(GUI_TRACE_ERROR, "CGfxWrapGL::InitView(): glCompileShader(FragmentShader): "+ux_string(pcInfoLog.get())+"\n");
        return;
    }

    // Loads the vertex shader in the same way
    m_uiVertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_uiVertexShaderHandle, 1, static_cast<const char**>(&cVertexShaderSource), NULL);
    glCompileShader(m_uiVertexShaderHandle);
    glGetShaderiv(m_uiVertexShaderHandle, GL_COMPILE_STATUS, &iShaderCompiled);
    if (false == iShaderCompiled)
    {
        // An error happened, first retrieve the length of the log message
        GLint iInfoLogLength;
        GLsizei iCharsWritten;
        glGetShaderiv(m_uiVertexShaderHandle, GL_INFO_LOG_LENGTH, &iInfoLogLength);

        // Allocate enough space for the message and retrieve it
        CGUIAutoPtr<ux_char> pcInfoLog(new ux_char[iInfoLogLength], true);
        glGetShaderInfoLog(m_uiVertexShaderHandle, iInfoLogLength, &iCharsWritten, pcInfoLog.get());

        GUILOG(GUI_TRACE_ERROR, "CGfxWrapGL::InitView(): glCompileShader(VertexShader): "+ux_string(pcInfoLog.get())+"\n");
        return;
    }

    // Create the shader program
    m_uiProgramObject = glCreateProgram();

    // Attach the fragment and vertex shaders to it
    glAttachShader(m_uiProgramObject, m_uiFragmentShaderHandle);
    glAttachShader(m_uiProgramObject, m_uiVertexShaderHandle);

    // Bind the custom vertex attribute "Vertex" to location GL_VERTEX_ARRAY
    glBindAttribLocation(m_uiProgramObject, GL_VERTEX_ARRAY, "Vertex");

    // Bind the custom vertex attribute "UV" to location GL_TEXTURE_COORD_ARRAY
    glBindAttribLocation(m_uiProgramObject, GL_TEXTURE_COORD_ARRAY, "UV");

    // Link the program
    glLinkProgram(m_uiProgramObject);

    // Check if linking succeeded in the same way we checked for compilation success
    GLint iShaderLinked = 0;
    glGetProgramiv(m_uiProgramObject, GL_LINK_STATUS, &iShaderLinked);

    if (false == iShaderLinked)
    {
        // An error happened, first retrieve the length of the log message
        GLint iInfoLogLength;
        GLsizei iCharsWritten;
        glGetProgramiv(m_uiProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

        // Allocate enough space for the message and retrieve it
        CGUIAutoPtr<ux_char> pcInfoLog(new ux_char[iInfoLogLength], true);
        glGetProgramInfoLog(m_uiProgramObject, iInfoLogLength, &iCharsWritten, pcInfoLog.get());

        GUILOG(GUI_TRACE_ERROR, "CGfxWrapGL::InitView(): glLinkProgram(): "+ux_string(pcInfoLog.get())+"\n");
        return;
    }

    // Actually use the created program
    glUseProgram(m_uiProgramObject);

    // Sets the sampler2D variable to the first texture unit
    glUniform1i(glGetUniformLocation(m_uiProgramObject, "TexSampler"), 0);

    // Sets the fragment texture env mode
    glUniform1i(glGetUniformLocation(m_uiProgramObject, "TextureEnvMode"), 1);
}


void gp_wrap_gl::MultMatrix(const Matrix3D_t& avMatrix)
{
    Matrix3D_t avResMatrix;
    memset(avResMatrix, 0, sizeof(Matrix3D_t));
    const Matrix3D_t& crkCurrentMatrix = GetMatrix();

    for(ux_uint i=0; i < cubNOFMatrixElements; i++)
    {
         for(ux_uint j=0; j < cubNOFMatrixElements; j++)
         {
             for(ux_uint k=0; k < cubNOFMatrixElements;k++)
             {
                avResMatrix[i][j] = avResMatrix[i][j] + avMatrix[i][k] * crkCurrentMatrix[k][j];
             }
         }
    }

    SetMatrix(avResMatrix);
}

void gp_wrap_gl::SetOrthoProjection(const ux_value& vLeft, const ux_value& vRight,
                                    const ux_value& vBottom, const ux_value& vTop,
                                    const ux_value& vNear, const ux_value& vFar)
{
    Matrix3D_t avMatrix;
    memset(avMatrix, 0, sizeof(Matrix3D_t));

    avMatrix[0][0]  = eC_Div(eC_FromInt(2), vRight-vLeft);

    avMatrix[1][1]  = eC_Div(eC_FromInt(2), vTop-vBottom);

    avMatrix[2][2] = eC_Div(eC_FromInt(-2), vFar-vNear);

    avMatrix[3][0] = -(eC_Div(vRight+vLeft, vRight-vLeft));
    avMatrix[3][1] = -(eC_Div(vTop+vBottom, vTop-vBottom));
    avMatrix[3][2] = -(eC_Div(vFar+vNear, vFar-vNear));
    avMatrix[3][3] = eC_FromInt(1);

    MultMatrix(avMatrix);
}

void gp_wrap_gl::SetFrustumProjection(const ux_value& vLeft, const ux_value& vRight,
                                        const ux_value& vBottom, const ux_value& vTop,
                                        const ux_value& vNear, const ux_value& vFar)
{
    Matrix3D_t avMatrix;
    memset(avMatrix, 0, sizeof(Matrix3D_t));

    avMatrix[0][0]  = eC_Div(eC_Mul(eC_FromInt(2), vNear), vRight-vLeft);

    avMatrix[1][1]  = eC_Div(eC_Mul(eC_FromInt(2), vNear), vTop-vBottom);

    avMatrix[2][0]  = eC_Div(vRight+vLeft, vRight-vLeft);
    avMatrix[2][1]  = eC_Div(vTop+vBottom, vTop-vBottom);
    avMatrix[2][2] = eC_Div(-(vFar+vNear), vFar-vNear);
    avMatrix[2][3] = eC_FromInt(-1);

    avMatrix[3][2] = eC_Div(-eC_Mul(eC_Mul(eC_FromInt(2), vFar), vNear), vFar-vNear);

    MultMatrix(avMatrix);
}

void gp_wrap_gl::SetRotation(const ux_value& vDegree, ux_value vX, ux_value vY, ux_value vZ)
{
    Matrix3D_t avMatrix;
    memset(avMatrix, 0, sizeof(Matrix3D_t));

    ux_value vMag = eC_Sqrt(eC_Mul(vX, vX)+
                            eC_Mul(vY, vY)+
                            eC_Mul(vZ, vZ));

    if (vMag == eC_FromInt(0))
    {
        vX = eC_FromInt(1);
        vY = eC_FromInt(0);
        vZ = eC_FromInt(0);
    }
    else if (vMag != 1.0)
    {
        vX = eC_Div(vX, vMag);
        vY = eC_Div(vY, vMag);
        vZ = eC_Div(vZ, vMag);
    }

    // Convert to radians for eC
    ux_value vRadians = eC_Mul( eC_RAD, vDegree);

    ux_value vC = eC_Cos(vRadians);
    ux_value vS = eC_Sin(vRadians);

    avMatrix[3][3] = eC_FromInt(1);
    ux_value eC_1 = eC_FromInt(1);

    // http://en.wikipedia.org/wiki/Rotation_matrix
    avMatrix[0][0] = eC_Mul(eC_Mul(vX, vX), (eC_1-vC))+vC;
    avMatrix[0][1] = eC_Mul(eC_Mul(vY, vX), (eC_1-vC))-eC_Mul(vZ, vS);
    avMatrix[0][2] = eC_Mul(eC_Mul(vX, vZ), (eC_1-vC))+eC_Mul(vY, vS);

    avMatrix[1][0] = eC_Mul(eC_Mul(vX, vY), (eC_1-vC))+eC_Mul(vZ, vS);
    avMatrix[1][1] = eC_Mul(eC_Mul(vY, vY), (eC_1-vC))+vC;
    avMatrix[1][2] = eC_Mul(eC_Mul(vY, vZ), (eC_1-vC))-eC_Mul(vX, vS);

    avMatrix[2][0] = eC_Mul(eC_Mul(vX, vZ), (eC_1-vC))-eC_Mul(vY, vS);
    avMatrix[2][1] = eC_Mul(eC_Mul(vY, vZ), (eC_1-vC))+eC_Mul(vX, vS);
    avMatrix[2][2] = eC_Mul(eC_Mul(vZ, vZ), (eC_1-vC))+vC;

    MultMatrix(avMatrix);
}

void gp_wrap_gl::SetTranslation(const ux_value& vXTranslate, const ux_value& vYTranslate, const ux_value& vZTranslate)
{
    Matrix3D_t avMatrix;
    memset(avMatrix, 0, sizeof(Matrix3D_t));

    avMatrix[0][0] = avMatrix[1][1] = avMatrix[2][2] = avMatrix[3][3] = eC_FromInt(1);

    avMatrix[3][0] = vXTranslate;
    avMatrix[3][1] = vYTranslate;
    avMatrix[3][2] = vZTranslate;

    MultMatrix(avMatrix);
}

void gp_wrap_gl::SetScaling(const ux_value& vXScale, const ux_value vYScale, const ux_value vZScale)
{
    Matrix3D_t avMatrix;
    memset(avMatrix, 0, sizeof(Matrix3D_t));

    avMatrix[0][0] = vXScale;
    avMatrix[1][1] = vYScale;
    avMatrix[2][2] = vZScale;
    avMatrix[3][3] = eC_FromInt(1);

    MultMatrix(avMatrix);
}

void gp_wrap_gl::SetUniformScaling(const ux_value& vScale)
{
    SetScaling(vScale, vScale, vScale);
}

void gp_wrap_gl::SetMatrix(const Matrix3D_t& avNewMatrix)
{
    switch (m_eActiveMatrix)
    {
    case MT_MODELVIEW:
        memcpy(m_avModelviewMatrix, avNewMatrix, sizeof(Matrix3D_t));
        glUniformMatrix4fv(glGetUniformLocation(m_uiProgramObject, "ModelviewMatrix"), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(m_avModelviewMatrix));
        break;

    case MT_PROJECTION:
        memcpy(m_avProjectionMatrix, avNewMatrix, sizeof(Matrix3D_t));
        glUniformMatrix4fv(glGetUniformLocation(m_uiProgramObject, "ProjectionMatrix"), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(m_avProjectionMatrix));
        break;

    default:
        break;
    };
}

const Matrix3D_t& gp_wrap_gl::GetMatrix() const
{
    switch (m_eActiveMatrix)
    {
    case MT_MODELVIEW:
        return m_avModelviewMatrix;
        break;

    case MT_PROJECTION:
        return m_avProjectionMatrix;
        break;

    default:
        return cavIdentityMatrix;
        break;
    };
}

void gp_wrap_gl::LoadIdentity()
{
    SetMatrix(cavIdentityMatrix);
}

void gp_wrap_gl::PushMatrix()
{
    Matrix3D_t *pavMatrix = new Matrix3D_t[1];
    switch (m_eActiveMatrix)
    {
    case MT_MODELVIEW:
        memcpy(*pavMatrix, m_avModelviewMatrix, sizeof(Matrix3D_t));
        m_kModelviewMatrixStack.AddAtEnd(pavMatrix);
        break;

    case MT_PROJECTION:
        memcpy(*pavMatrix, m_avProjectionMatrix, sizeof(Matrix3D_t));
        m_kProjectionMatrixStack.AddAtEnd(pavMatrix);
        break;

    default:
        break;
    };
}

void gp_wrap_gl::PopMatrix()
{
    Matrix3D_t *pavMatrix = NULL;

    switch (m_eActiveMatrix)
    {
    case MT_MODELVIEW:
        if (true == m_kModelviewMatrixStack.RemoveEnd(pavMatrix))
        {
            if (NULL != pavMatrix)
            {
                SetMatrix(*pavMatrix);
                delete[] pavMatrix;
            }
        }
        break;

    case MT_PROJECTION:
        if (true == m_kProjectionMatrixStack.RemoveEnd(pavMatrix))
        {
            if (NULL != pavMatrix)
            {
                SetMatrix(*pavMatrix);
                delete[] pavMatrix;
            }
        }
        break;

    default:
        break;
    };
}
#endif // #if defined GUILIANI_USE_OGLES2

// ---------------- CGUIImageDataGL --------------------------

ux_image_data_gl::ux_image_data_gl() :/*
    CGUIImageData(),*/
    m_uiTextureID(0), m_uiTextureWidth(0), m_uiTextureHeight(0) { }

ux_image_data_gl::~ux_image_data_gl()
{
}
