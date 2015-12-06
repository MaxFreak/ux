//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_GP_WRAP_OGL_SDL_H
#define UX_GP_WRAP_OGL_SDL_H

#include <SDL/SDL.h>
#include <OPENGL/gl.h>

#include "gp_wrap_ogl.h"
#include "ux_types_and_defs.h"

class gp_wrap_ogl_sdl : public gp_wrap_ogl
{
public:
    gp_wrap_ogl_sdl(ux_uint uiScreenWidth, ux_uint uiScreenHeight, ux_bool bResizable);

    ~gp_wrap_ogl_sdl(void);

    void set_screen_size(const ux_uint &uiWidth, const ux_uint &uiHeight);

    ux_bool init(const ux_uint &uiScreenWidth, const ux_uint &uiScreenHeight);

    void de_init(void);

    ux_bool refresh(const refresh_type eRefreshCallType);

private:
    /// SDL surface is the pointer to the screen.
    SDL_Surface *ms_pSDLSurface;

    /// Window is resizable
    ux_bool m_bResizable;
};


#endif //UX_GP_WRAP_OGL_SDL_H
