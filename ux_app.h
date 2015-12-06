//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_SDL_TEST_H
#define UX_SDL_TEST_H

#include <SDL/SDL.h>
#include <OPENGL/gl.h>
#include <OPENGL/glu.h>
#include "in_device.h"

class ux_app
{
public:
    ux_app();
    virtual ~ux_app();
    bool handle_event(in_device &input_device);
    void draw_screen( void );

    int main_loop(in_device &input_device);
private:
};


#endif //UX_SDL_TEST_H
