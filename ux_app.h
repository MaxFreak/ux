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

namespace ux
{

class ux_app
{
public:
    ux_app(int argc, char* args[]);

    virtual ~ux_app();

    bool handle_event(in_device &input_device);

    void draw_screen(gp::gp_wrap &graphic_wrap);

    int main_loop(gp::gp_wrap &graphic_wrap, in_device &input_device);

private:
public:
    const ux_string &get_exe_file_path() const
    {
        return m_exe_file_path;
    }

    void set_exe_file_path(const ux_string &exe_file_path)
    {
        m_exe_file_path = exe_file_path;
    }

    const ux_string &get_exe_path() const
    {
        return m_exe_path;
    }

    void set_exe_path(const ux_string &exe_path)
    {
        m_exe_path = exe_path;
    }

    const ux_string &get_resources_path() const
    {
        return m_resources_path;
    }

    void set_resources_path(const ux_string &resources_path)
    {
        m_resources_path = resources_path;
    }

private:
    ux_string m_exe_file_path;
    ux_string m_exe_path;
    ux_string m_resources_path;
    ux_value m_xpos;
    ux_value m_ypos;
    ux_value m_size;
    ux_ubyte m_color;
};

} // namespace ux

#endif //UX_SDL_TEST_H
