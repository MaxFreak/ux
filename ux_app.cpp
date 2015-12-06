//
// Created by Bernd Lappas on 29.11.15.
//

#include "ux_app.h"
#include "in_event.h"

ux_app::ux_app()
{
}

ux_app::~ux_app()
{
}

void ux_app::draw_screen(void )
{
    static GLubyte red[]    = { 255,   0,   0, 255 };
    static GLubyte green[]  = {   0, 255,   0, 255 };
    static GLubyte blue[]   = {   0,   0, 255, 255 };
    static GLubyte white[]  = { 255, 255, 255, 255 };
    static GLubyte yellow[] = {   0, 255, 255, 255 };
    static GLubyte black[]  = {   0,   0,   0, 255 };
    static GLubyte orange[] = { 255, 255,   0, 255 };
    static GLubyte purple[] = { 255,   0, 255,   0 };

    /* Clear the color and depth buffers. */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Send our triangle data to the pipeline. */
    glBegin( GL_TRIANGLES );

    static GLfloat v0[] = { 0.0f, 0.0f,  0.0f };
    static GLfloat v1[] = {  100.0f, 0.0f,  0.0f };
    static GLfloat v2[] = {  100.0f,  100.0f,  0.0f };
    static GLfloat v3[] = { 0.0f,  100.0f,  0.0f };

    glColor4ubv( red );
    glVertex3fv( v0 );
    glColor4ubv( green );
    glVertex3fv( v1 );
    glColor4ubv( blue );
    glVertex3fv( v2 );
    glColor4ubv( white );
    glVertex3fv( v3 );
    glColor4ubv( white );
    glVertex3fv( v2 );
    glColor4ubv( white );
    glVertex3fv( v1 );

    glEnd( );

    SDL_GL_SwapBuffers( );
}

bool ux_app::handle_event(in_device &input_device)
{
    in_event_keyboard *key_ev = nullptr;
    in_event_key_type_e key_id_e = in_event_key_type_e::GK_NONE;

    // Process incoming events.
    in_event *ev = input_device.GetEvent(60);
    if (ev == nullptr)
        return false;

    switch (ev->GetType())
    {
        case in_event_type_e::ET_KEYDOWN:
            key_ev = static_cast<in_event_keyboard*>(ev);
            key_id_e = key_ev->GetKeyIdentifier();
            if (key_id_e == in_event_key_type_e::GK_ESCAPE)
                return true;
            break;
        default:
            break;
    }

    return false;
}

int ux_app::main_loop(in_device &input_device)
{
//    Now we want to begin our normal app process--
//    an event loop with redrawing.

    bool quit = false;

    while( !quit )
    {
        // Process incoming events.
        quit = handle_event(input_device);

        // Draw the screen.
        draw_screen( );
    }

    return 0;
}
