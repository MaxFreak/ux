//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_SDL_TEST_H
#define UX_SDL_TEST_H

#include <SDL/SDL.h>
#include <OPENGL/gl.h>
#include <OPENGL/glu.h>

#include "gp_wrap.h"
#include "in_device.h"

class ux_app
{
public:
    ux_app();
    virtual ~ux_app();
    bool handle_event(in_device &input_device);
    void draw_screen( gp_wrap &graphic_wrap );

    int main_loop(gp_wrap &graphic_wrap, in_device &input_device);
private:
    ux_value m_xpos;
    ux_value m_ypos;
    ux_value m_size;
    ux_ubyte m_color;
};


#endif //UX_SDL_TEST_H
