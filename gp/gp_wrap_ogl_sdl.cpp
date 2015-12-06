//
// Created by Bernd Lappas on 29.11.15.
//

#include "gp_wrap_ogl_sdl.h"

gp_wrap_ogl_sdl::gp_wrap_ogl_sdl(ux_uint uiScreenWidth, ux_uint uiScreenHeight, ux_bool bResizable) :
    gp_wrap_ogl(),
    m_bResizable(bResizable)
{
    init(uiScreenWidth, uiScreenHeight);

    if (ms_pSDLSurface)
    {
        if (uiScreenHeight == 0 && uiScreenWidth == 0)
        {
            create_screen(ms_pSDLSurface->w, ms_pSDLSurface->h);
        }
        else
        {
            create_screen(uiScreenWidth, uiScreenHeight);
        }
    }
}

gp_wrap_ogl_sdl::~gp_wrap_ogl_sdl(void)
{
    de_init();
}

ux_bool gp_wrap_ogl_sdl::init(const ux_uint &uiScreenWidth, const ux_uint &uiScreenHeight)
{
    if (SDL_Init(0) == (-1))
    {
        UX_TRACE_ERROR("CGfxWrapOGLSDL::init: SDL_Init() failed: " + ux_string(SDL_GetError()) + ".\n");
        return false;
    }

    Uint32 uiFlags = SDL_OPENGL | SDL_ANYFORMAT | SDL_DOUBLEBUF;

    if ((uiScreenHeight != 0 || uiScreenWidth != 0) && m_bResizable)
    {
        //Resizable window
        uiFlags |= SDL_RESIZABLE;
    }

    if ((ms_pSDLSurface = SDL_SetVideoMode(uiScreenWidth, uiScreenHeight, 0, uiFlags)) == NULL)
    {
        UX_TRACE_ERROR("CGfxWrapOGLSDL::init: SDL_SetVideoMode() failed: " + ux_string(SDL_GetError()) + ".\n");
        return false;
    }

    //get adresses to vbo extensions.
#if !defined SYSTEM_NAME_DARWIN
    glBindBuffer_ux               = (PFNGLBINDBUFFERPROC) SDL_GL_GetProcAddress("glBindBufferARB");
        glBufferData_ux               = (PFNGLBUFFERDATAPROC) SDL_GL_GetProcAddress("glBufferDataARB");
        glGenBuffers_ux              = (PFNGLGENBUFFERSPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
        glDeleteBuffers_ux            = (PFNGLDELETEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");

        m_bFBOSupported = true;
        // get addresses to fbo extensions
        glGenFramebuffers_ux         = (PFNGLGENFRAMEBUFFERSPROC) SDL_GL_GetProcAddress("glGenFramebuffersEXT");
        glBindFramebuffer_ux         = (PFNGLBINDFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glBindFramebufferEXT");
        glFramebufferTexture2D_ux     = (PFNGLFRAMEBUFFERTEXTURE2DPROC) SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
        glCheckFramebufferStatus_ux   = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
        glDeleteFramebuffers_ux       = (PFNGLDELETEFRAMEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
#endif

    if (!glBindBuffer_ux || !glBufferData_ux || !glGenBuffers_ux || !glDeleteBuffers_ux)
    {
        UX_THROW_EXCEPTION(ux_exception(), "CGfxWrapOGLSDL::init: Could not initialize VBO extensions.\n");
    }

    if (!glGenFramebuffers_ux || !glBindFramebuffer_ux || !glFramebufferTexture2D_ux || !glCheckFramebufferStatus_ux ||
        !glDeleteFramebuffers_ux)
    {
        m_bFBOSupported = false;
        UX_TRACE_DEBUG(
            "CGfxWrapOGLSDL::init: Could not initialize FBO extensions. Using fallback solution which does not support Alpha for BitmapPlanes.\n");
    }

    if(!gp_wrap_gl::init()) // Will be called now (not at the start of init) because SDL should be initialized before!
        return false;

    init_view();

    return true;
}

void gp_wrap_ogl_sdl::de_init(void)
{
    SDL_Quit();
    gp_wrap_gl::de_init();
}

void gp_wrap_ogl_sdl::set_screen_size(const ux_uint &width, const ux_uint &height)
{
    // NOTE/TODO:
    // This re-creates the surface OGL renders into; it is initially filled black.
    // To avoid flickering, it sould be filled with a copy of the old GUI before
    // GETGUI.InvalidateArea() is called by the input wrapper, but surfaces created
    // with SDL_OPENGL can not be accessed for blitting.
    // Can this be done?

    if ((ms_pSDLSurface = SDL_SetVideoMode(width, height, 0,
                                           SDL_OPENGL | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_RESIZABLE)) == NULL)
    {
        UX_TRACE_ERROR(
            "CGfxWrapOGLSDL::set_screen_size: SDL_SetVideoMode() failed: " + ux_string(SDL_GetError()) + ".\n");
    }
    else
    {
        gp_wrap_gl::set_screen_size(width, height);
    }
}

ux_bool gp_wrap_ogl_sdl::refresh(const refresh_type eRefreshCallType)
{
    if (eRefreshCallType == refresh_single || eRefreshCallType == refresh_last_frame)
    {
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        m_vDepthCorrection = 0;
        SDL_GL_SwapBuffers();
    }
    return true;
}


