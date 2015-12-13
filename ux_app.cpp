//
// Created by Bernd Lappas on 29.11.15.
//

#include "ux_app.h"

#include "in_event.h"
#include "ux_resources.h"

#include <boost/filesystem.hpp>

using namespace ux;
using namespace ux::gp;

ux_app::ux_app(int argc, char* args[]) :
    m_exe_file_path(""),
    m_exe_path(""),
    m_xpos(100),
    m_ypos(100),
    m_size(100),
    m_color(0x80)
{
    using namespace boost::filesystem;

    m_exe_file_path = args[0];
    m_exe_path = initial_path().generic_string();
}

ux_app::~ux_app()
{
}

void ux_app::draw_screen(gp_wrap &graphic_wrap )
{
    // Clear screen
    graphic_wrap.set_foreground_color(0xff000000);
    graphic_wrap.filled_rect(0, 0, graphic_wrap.get_physical_screen_width(), graphic_wrap.get_physical_screen_height());

    graphic_wrap.set_foreground_color(m_color);
    graphic_wrap.set_foreground_color(m_color, m_color, m_color, 0xff);
    graphic_wrap.filled_rect(m_xpos, m_ypos, m_xpos + m_size, m_ypos + m_size);
    graphic_wrap.redraw();
//    SDL_GL_SwapBuffers( );
}

bool ux_app::handle_event(in_device &input_device)
{
    in_event_keyboard *key_ev = nullptr;
    in_event_key_type_e key_id_e = in_event_key_type_e::GK_NONE;

    // Process incoming events.
    in_event *ev = input_device.GetEvent(10);
    if (ev == nullptr)
        return false;

    switch (ev->GetType())
    {
        case in_event_type_e::ET_KEYDOWN:
        {
            key_ev = static_cast<in_event_keyboard *>(ev);
            in_event_key_type_e key_e = key_ev->GetKeyIdentifier();
            ux_uint key_content = key_ev->GetKeyContent();
            ux_uint key_modifier = key_ev->GetModifiers();
     //       std::cout << "Key: " << key_e << " Content: " << key_content << " Modifier: " << key_modifier << "\n";

            switch (key_e)
            {
                case in_event_key_type_e::GK_ESCAPE:
                    return true;
                case in_event_key_type_e::GK_MINUS:
                    m_size--;
                    return false;
                case in_event_key_type_e::GK_PLUS:
                    m_size++;
                    return false;
                case in_event_key_type_e::GK_LEFT:
                    m_xpos--;
                    return false;
                case in_event_key_type_e::GK_RIGHT:
                    m_xpos++;
                    return false;
                case in_event_key_type_e::GK_UP:
                    m_ypos--;
                    return false;
                case in_event_key_type_e::GK_DOWN:
                    m_ypos++;
                case in_event_key_type_e::GK_NONE:
                {
                    switch (key_content)
                    {
                        case '*':
                            m_color++;
                            return false;
                        case '/':
                            m_color--;
                            return false;
                    }
                    return false;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return false;
}

int ux_app::main_loop(gp_wrap &graphic_wrap, in_device &input_device)
{
    ux_resources app_resources;

    auto img01 = app_resources.add_resource("Hallo 01", resource_type::image);
    auto img02 = app_resources.add_resource("Hallo 02", resource_type::image);
    auto img03 = app_resources.add_resource("Hallo 03", resource_type::image);
    auto img04 = app_resources.add_resource("Hallo 04", resource_type::image);
    auto fnt01 = app_resources.add_resource("Font 01", resource_type::font);
    auto snd01 = app_resources.add_resource("Sound 01", resource_type::sound);
    auto oth01 = app_resources.add_resource("Other 01", resource_type::other);
    auto emp01 = app_resources.add_resource("Empty 01", resource_type::empty);

    auto img_data02 = app_resources.get_resource(img02);
    auto fnt_data = app_resources.get_resource(fnt01);

    app_resources.remove_resource("Sound 01");
    app_resources.remove_resource("Hallo 03");

//    Now we want to begin our normal app process--
//    an event loop with redrawing.

    bool quit = false;

    while( !quit )
    {
        // Process incoming events.
        quit = handle_event(input_device);

        // Draw the screen.
        draw_screen(graphic_wrap);
    }

    return 0;
}
