//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_GP_WRAP_GL_H
#define UX_GP_WRAP_GL_H

#include <vector>

#include "gp_wrap.h"
#include  "gl_api_mappings.h"

#ifdef GP_GLDEBUG
/// Helper macro for convenient checks for OpenGL errors in debug builds.
#define CHECK_GL_ERROR  gp_wrap_gl::CheckGLError(__FUNCTION__);
#else
#define CHECK_GL_ERROR
#endif

/// OpenGL Version of CGUIImageData for textures.

class ux_image_data_gl/*:
    public CGUIImageData*/
{
    // Make CGfxWrapGL friend to get access to get_texture_ID
    friend class CGfxWrapGL;

public:
    /** CGUIImageData constructor */
    ux_image_data_gl();

    /** CGUIImageData destrcutor */
    ~ux_image_data_gl();

    inline void SetID(const ux_uint uiTextureID){ m_uiTextureID = uiTextureID;}///<@param uiTextureID The texture ID
    inline void SetTextureWidth(const ux_uint uiTexWidth){ m_uiTextureWidth = uiTexWidth;}///<@param uiTexWidth The texture width
    inline void SetTextureHeight(const ux_uint uiTexHeight){ m_uiTextureHeight = uiTexHeight;}///<@param uiTexHeight The texture height

    inline ux_uint GetTextureWidth() const {return m_uiTextureWidth;}///<@return The texture width
    inline ux_uint GetTextureHeight() const {return m_uiTextureHeight;}///<@return The texture height

    inline const ux_uint GetTextureID() const {return m_uiTextureID;}///<@return The texture ID

private:
    /** Copy-constructor
        Dummy implementation to avoid unintended use of compiler-generated default
    */
    ux_image_data_gl(const ux_image_data_gl & kSource);

    /** Operator= method.
        Dummy implementation to avoid unintended use of compiler-generated default
    */
    ux_image_data_gl & operator=(const ux_image_data_gl & kSource);


    ux_uint m_uiTextureID; // OpenTexture identifier
    ux_uint m_uiTextureWidth; // texture size for calculating texture coordinates
    ux_uint m_uiTextureHeight;
};

class gp_wrap_gl: public gp_wrap
{

public:
    /// Offset to get from OpenGL's coordinates to the pixel center.
    /// Note: OpenGL Pixel Coordinates do not refer to the "center" of the pixel, but to the edges between them.
    /// This means 0/0 would not point to the center of the top/left pixel, but instead to the top/left corner of this pixel
    /// Therefore we move the coordinates to the center of the pixel by adding 0.375 pixels because of better compatibility
    /// to more graphic cards.
    static const ux_value cvPixelCenterOffset;
    // ---------------------------------------------------------------
    // General functionality
    // ---------------------------------------------------------------

    void create_screen(const ux_uint &uiWidth, const ux_uint &uiHeight);

    virtual ux_uint get_virtual_screen_width();
    virtual ux_uint get_physical_screen_width();
    virtual ux_uint get_virtual_screen_height();
    virtual ux_uint get_physical_screen_height();
    virtual void set_screen_size(const ux_uint &uiWidth, const ux_uint &uiHeight);

    // ---------------------------------------------------------------
    // Drawing functions
    // ---------------------------------------------------------------

    /** Sets the foreground color via r, g and b values and an alpha value
    @param red Red intensity within a range of 0 - 255
    @param green Green intensity within a range of 0 - 255
    @param blue Blue intensity within a range of 0 - 255
    @param alpha alpha value
    */
    virtual void set_foreground_color_impl(const ux_ubyte &red, const ux_ubyte &green, const ux_ubyte &blue,
                                           const ux_ubyte &alpha = 255);

    virtual ux_value set_line_width(const ux_value &vWidth);

    /** Draws a line from (vX1, vY1) to (vX2, vY2)
    @param vX1 X-Coordinate of starting point
    @param vY1 Y-Coordinate of starting point
    @param vX2 X-Coordinate of ending point
    @param vY2 Y-Coordinate of ending point
    */
    virtual void line(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2);

    /** Draws a rectangle with the top left edge at (vX1, vY1) and bottom right edge at (vX2, vY2)
    @param vX1 X-Coordinate of top left corner
    @param vY1 Y-Coordinate of top left corner
    @param vX2 X-Coordinate of bottom right corner
    @param vY2 Y-Coordinate of bottom right corner
    */
    virtual void rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2);

    /** Draws a filled rectangle with the top left edge at (vX1, vY1) and bottom right edge at (vX2, vY2)
    @param vX1 X-Coordinate of top left corner
    @param vY1 Y-Coordinate of top left corner
    @param vX2 X-Coordinate of bottom right corner
    @param vY2 Y-Coordinate of bottom right corner
    */
    virtual void filled_rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2);

    /** Draws an ellipse which is complete inside the rectangle described through
    the rectangle the top left edge at (vX1, vY1) and bottom right edge at (vX2, vY2).
    The ellipse will be filled out if the last parameter is true
    @param vX1 X-Coordinate of bounding rectangle's top left corner
    @param vY1 Y-Coordinate of bounding rectangle's top left corner
    @param vX2 X-Coordinate of bounding rectangle's bottom right corner
    @param vY2 Y-Coordinate of bounding rectangle's bottom right corner
    @param bFilled Specifies whether the ellipse will be drawn outlined (false) or filled (true)
    */
    virtual void ellipse(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2,
                         const ux_bool &bFilled);

    // ---------------------------------------------------------------
    // Clipping and invalidated rectangles
    // ---------------------------------------------------------------

    /** Sets a clipping rectangle.
        All drawing operations will be limited to the region within the clipping rectangle. This is very
        useful for advanded features such as soft-scrolling. Note that the clipping rectangle is not
        allowed to exceed the invalidated rectangle, since this would lead to visual artifacts.
        @param rkNewClipRect A normalised rectangle that includes all pixels that could be drawn
               by the functions of the graphics wrapper. The rectangle includes the
               border. All pixels outside will be left untouched.
    */
    virtual void set_cliprect(const geo::FloatRect &rkNewClipRect);

    virtual ux_bool image_exists(const ImageResource_t eID) const
    {
//        ux_uint uiCurImage = MAP_IMGID_TO_ARRAY(eID);
//        return m_apTextures[uiCurImage] != NULL && (m_apTextures[uiCurImage]->GetData() != NULL || m_apTextures[uiCurImage]->get_texture_ID() != 0);
        return false;
    }

    /** Gets pointer to an image data object.
    @param eImageID Image data object to be returned.
    @return Pointer to image data object.
    */
//    ux_image_data_gl* GetImageData(const ImageResource_t &eImageID);

    /** Sets the default 2D view used by the gfx wrapper for 2D drawing operations.
        This method uses the dimensions returned by get_virtual_screen_width() and get_virtual_screen_height().
    */
    void set_2D_view();

    /** Sets the default 2D view used by the gfx wrapper for 2D drawing operations.
        @param uiWidth Width of Viewport
        @param uiHeight Height of Viewport
    */
    void set_2D_view(const ux_uint uiWidth, const ux_uint uiHeight);

    /** Checks for occurency of OpenGL Erros.
        All OpenGL errors (error code and text) since the last call of this function (or glGetError) are log in the Guiliani Log.
        @param kCallingMethod String which will be added to the Log entry.
        @return True if successful, False otherwise
    */
    static ux_bool check_GL_error(const ux_string &kCallingMethod);

    /** Returns the TextureID and generates the texture if needed
        @param eImageID The image ID
        @return The texture ID
    */
    const ux_uint get_texture_ID(const ImageResource_t &eImageID);

    /**
     * Applies a perspective projection matrix to current GL_PROJECTION. Old
     * projection matrix will be overwritten. Projection matrices are used
     * to project 3D space coordinates to a 2D screen.
     *
     * OpenGL and OpenGL-ES provides projection matrix creation for parallel
     * projection mode (glOrtho), but for perspective projection invokation
     * of the GLU library is needed. To avoid this dependency, this
     * implementation provides this.
     *
     * The matrix is calculated the following way:
     *
     *     (1/w) ( 0 ) (       0       ) (       0       )
     * M = ( 0 ) (1/h) (       0       ) (       0       )
     *     ( 0 ) ( 0 ) ( -(near+far)/d ) ( -2*near*far/d )
     *     ( 0 ) ( 0 ) (      -1       ) (       0       )
     *
     * where:
     *
     * h = tan(fovy/2)
     * w = aspectRatio * h
     * d = far - near
     *
     * @param vFovy Field of view in the vertical direction in DEG [0; 90].
     * @param vAspect Aspect ratio of the viewport, which is width divided by
     *  height.
     * @param vNear Distance to front clipping plane.
     * @param vFar Distance to back clipping plane.
     */
    void set_perspective_projection(const ux_value &vFovy, const ux_value &vAspect, const ux_value &vNear,
                                    const ux_value &vFar);

    ///Helper for initialization
    virtual ux_bool init_view();

    /** Returns the currently used vertex buffer object.
        @return Vertex buffer object.
    */
    inline ux_uint get_VBO() const {return m_uiVBO;}

    /** Returns the currently used UV vertex buffer object.
        @return UV vertex buffer object.
    */
    inline ux_uint get_UVVBO() const {return m_uiUVVBO;}

#if defined GUILIANI_USE_OGL
    #if !defined SYSTEM_NAME_DARWIN
        /** Function pointers for Vertex Buffer Objects. */
        static PFNGLBINDBUFFERPROC              glBindBuffer_ux;
        static PFNGLBUFFERDATAPROC              glBufferData_ux;
        static PFNGLGENBUFFERSPROC              glGenBuffers_ux;
        static PFNGLDELETEBUFFERSPROC           glDeleteBuffers_ux;

        /** Function pointers for Frame Buffer Objects. */
        static PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers_ux;
        static PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer_ux;
        static PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D_ux;
        static PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus_ux;
        static PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers_ux;
    #endif
#endif

#if defined GUILIANI_USE_OGLES2
    // Initializes default shader objects for OpenGL(ES)
    void InitShaders();

    /** Matrix multiplication.
        The matrix given in avMatrix will be multiplied with the matrix set by SetMatrix(), which will also
        be used to store the result of the multiplication.  ActiveMatrix = avMatrix * ActiveMatrix
        @param avMatrix Matrix to multiply the current matrix with.
    */
    void MultMatrix(const Matrix3D_t& avMatrix);

    ///Replacement for glOrtho in OpenGL ES 2.x
    void SetOrthoProjection(const ux_value& vLeft, const ux_value& vRight,
        const ux_value& vBottom, const ux_value& vTop,
        const ux_value& vNear, const ux_value& vFar);

    ///Replacement for glFrustum in OpenGL ES 2.x
    void SetFrustumProjection(const ux_value& vLeft, const ux_value& vRight,
        const ux_value& vBottom, const ux_value& vTop,
        const ux_value& vNear, const ux_value& vFar);

    ///Replacement for glRotate in OpenGL ES 2.x
    void SetRotation(const ux_value& vDegrees, ux_value vX, ux_value vY, ux_value vZ);

    ///Replacement for glTranslate in OpenGL ES 2.x
    void SetTranslation(const ux_value& vXTranslate, const ux_value& vYTranslate, const ux_value& vZTranslate);

    ///Replacement for glScale in OpenGL ES 2.x
    void SetScaling(const ux_value& vXScale, const ux_value vYScale, const ux_value vZScale);

    ///Calls SetScaling with the same parameter for the whole matrix
    void SetUniformScaling(const ux_value& vScale);

    /** Returns the program object to get access to the shader program.
        @return Program object.
    */
    inline ux_uint GetProgramObject() const {return m_uiProgramObject;}

    /** Replaces the currently used matrix with ne new one.
        @param avNewMatrix New matrix to set.
    */
    void SetMatrix(const Matrix3D_t& avNewMatrix);

    /** Returns the currently active matrix (or identity matrix in default switch case).
        @return Active matrix.
    */
    const Matrix3D_t& GetMatrix() const;

    /** Pushes the currently active Matrix onto the stack. */
    void PushMatrix();

    /** Pops the last used active Matrix from stack. */
    void PopMatrix();

    /** Available matrix types */
    enum MatrixType_t
    {
        MT_PROJECTION,
        MT_MODELVIEW
    };

    /** Sets the matrix mode
        @param eNewActiveMatrix Matrix to set active.
    */
    inline void SetMatrixMode(const MatrixType_t& eNewActiveMatrix) {m_eActiveMatrix = eNewActiveMatrix;}

    /** Sets identity matrix to currently active one. */
    void LoadIdentity();
#endif //#if defined GUILIANI_USE_OGLES2

protected:
    // ---------------------------------------------------------------
    // Construction / destruction
    // ---------------------------------------------------------------

    /// Constructor
    gp_wrap_gl(void);

    /// Destructor
    virtual ~gp_wrap_gl(void);

//    virtual CGUIBitmapPlane* CreateGfxDependentBitmap(
//        GUIBitmapPlaneID_t uiNewBitmapID,
//        const CGUIObject& rkParentObject,
//        const ux_value &vWidth, const ux_value &vHeight);

    /** Restores the Backbuffer of the display after it has been used for texture rendering
        Always begins in LOWER LEFT of screen!
        @param vWidth of the area to restore.
        @param vHeight of the area to restore.
    */
    virtual void restore_back_buffer(const ux_value vWidth, const ux_value vHeight);

    /** Loads an image.
    This loads an image from the given path and associates it with an Image-Identifier.
    @param psPath Complete path and filename of the image to be loaded.
    @param eID ID by which the image will later be accessible.
    @return True if the image was successfully loaded, False otherwise
    */
    virtual void load_img_impl(const ux_string &psPath, const ImageResource_t &eID);

    /** Unloads an image.
    Unloads the image associated with the supplied Image-identifier and frees the memory formerly
    occupied by this image.
    @param eID Identifier of the image to be unloaded.
    */
    virtual void unload_img(const ImageResource_t &eID);

    ux_bool set_nof_imagesImpl(const ux_uint uiNOFImages);

    // ---------------------------------------------------------------
    // Image-access
    // ---------------------------------------------------------------

    virtual void blit_img_ext_impl(const ImageResource_t &eID, const ux_uint &uiSrcX, const ux_uint &uiSrcY,
                                   const ux_uint &uiSrcWidth, const ux_uint &uiSrcHeight, const ux_value &vDestX,
                                   const ux_value &vDestY, const ux_value &vDestWidth, const ux_value &vDestHeight,
                                   const ux_ubyte &alpha = 255, const ux_value &vAngle = 0,
                                   const ux_value &vRotCenterX = 0.5, const ux_value &vRotCenterY = 0.5);

    /** Gets the width of an image.
    @param eID Identifier of the image.
    @return The width of the image in pixels.
    */
    virtual ux_uint get_img_width_impl(const ImageResource_t &eID) const;

    /** Gets the height of an image.
    @param eID Identifier of the image.
    @return The height of the image in pixels.
    */
    virtual ux_uint get_img_height_impl(const ImageResource_t &eID) const;

    /** Returns the amount of memory taken up by an image.
        @param eID ID of the image to be measured.
        @return Size of the image in byte.
      */
    virtual ux_uint get_image_size(const ImageResource_t &eID) const;

    /** Draws a rectangle
        @param vX1 X position of corner 1
        @param vY1 Y position of corner 1
        @param vX2 X position of corner 2
        @param vY2 Y position of corner 2
        @param bFilled Whether to draw a filled rectangle
      */
    void draw_rect(const ux_value &vX1, const ux_value &vY1, const ux_value &vX2, const ux_value &vY2,
                   const ux_bool bFilled);

    /** Generates a texture of matching (power of 2) size for the given image ID and copies the image into it
    @param eImageID ImageID for which the texture should be created.
    */
    void generate_texture(const ImageResource_t &eImageID);

    /** Accessing the Z-Correction for current render target.
      * Render targets may be the screen or any BitmapPlane.
      *  @return returns the current z-correction for the active render target.
      */
    ux_value get_z_correction();

    /** Increases the Z-Correction for the current render target.
      * The render target may be the screen or any BitmapPlane.
      */
    void increase_z_correction();

    /** This set_cliprect version is used for an anti-clockwise rotation of the GUI. E.g. in GfxWrapEAGL implementation, if the iOS device is rotated.
        @param rkNewClipRect A normalised rectangle that includes all pixels that could be drawn
               by the functions of the graphics wrapper. The rectangle includes the
               border. All pixels outside will be left untouched.
        @param bRotate Set this true to set glScissor rotated anti-clockwise.
    */
    void set_cliprect(const geo::FloatRect &rkNewClipRect, const ux_bool &bRotate);

    ux_uint m_uiScreenWidth;    ///< Screen width
    ux_uint m_uiScreenHeight;   ///< Screen height

    ux_value m_vRed;            ///< Color values for red channel as value in the float range between 0 and 1
    ux_value m_vGreen;          ///< Color values for green channel as value in the float range between 0 and 1
    ux_value m_vBlue;           ///< Color values for blue channel as value in the float range between 0 and 1
    ux_value m_vAlpha;          ///< Color values for alpha channel as value in the float range between 0 and 1

    std::vector<ux_image_data_gl *> m_apTextures; ///< Array holding the textures

    /// Z-Correction of the ScreenBuffer
    ux_value m_vDepthCorrection;

    /// transition matrix
    Matrix3D_t m_avTransMat;

    /// fovy
    ux_value m_vFovy;

    /// aspect
    ux_value m_vAspect;

    /// near
    ux_value m_vNear;

    /// far
    ux_value m_vFar;

    /// vertex buffer object
    ux_uint m_uiVBO;

    /// UV vertex buffer object
    ux_uint m_uiUVVBO;

#if defined UX_USE_OGLES2
    /// projection matrix
    Matrix3D_t m_avProjectionMatrix;

    /// modelview matrix
    Matrix3D_t m_avModelviewMatrix;

    /// modelview matrix stack
    eC_TListDoubleLinked<Matrix3D_t*> m_kModelviewMatrixStack;

    /// projection matrix stack
    eC_TListDoubleLinked<Matrix3D_t*> m_kProjectionMatrixStack;

    /// The vertex shader OpenGL handle
    ux_uint m_uiVertexShaderHandle;
    /// fragment shader OpenGL handle
    ux_uint m_uiFragmentShaderHandle;

    /// program object.
    ux_uint m_uiProgramObject;

    ///active matrix
    MatrixType_t m_eActiveMatrix;
#endif //#if defined GUILIANI_USE_OGLES2

protected:
    /** Method to initialize the graphic library
    @return returns True if the initialisation succeded, False otherwise*/
    ux_bool init(void);

    /** Method to give the graphic library the opportunity to clean up */
    void de_init(void);
};


#endif //UX_GP_WRAP_GL_H
