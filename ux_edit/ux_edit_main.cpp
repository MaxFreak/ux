#include <iostream>
#include <boost/filesystem.hpp>


#include <gp_image_decoder_png.h>
#include "ui_object.h"
#include "ui_object_container.h"
#include "ui_history.h"
#include "gp_wrap_ogl_sdl.h"
#include "gp_image_decoder.h"
#include "gp_image_loader.h"

#include "ux_config.h"

#include "SDL.h"

//
//#include <GLUT/glut.h>
//#include <OpenGL/glext.h>
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>

//#include <GL/glut.h>
//#include <GL/glext.h>
//#include <GL/gl.h>
//#include <GL/glu.h>

#include "common_helpers.h"
#include "ux_app.h"
#include "in_device_sdl.h"

template<typename T>
void print_to(const T &X, ostream &Out, size_t Position)
{
    Out << string(Position, ' ') << X;
}

template<typename T>
void draw(const T &X, ostream &Out, geo::int_rect Position)
{
    Position.print_to(Out, 2);
}

// class ui_object

// class ui_object_container

// class ui_history

class my_class_t
{
/* ... */
};

void print_to(const my_class_t&, std::ostream& out, size_t position)
{
    out << std::string(position, ' ') << "my_class_t";
}

void draw(const my_class_t&, std::ostream& Out, geo::int_rect Position)
{
}


int main(int argc, char* args[])
{
    ux_string path_to_exe = args[0];

    using std::cout;
    using std::endl;

    cout << "ux_edit" << endl << endl;

#ifdef USE_SDL
#ifdef USE_GL

    using namespace ux;
    using namespace ux::gp;

    ux_app app(argc, args);
//    app.main_test();

    gp_image_decoder_png img_dec_png;
    gp_image_loader img_loader;
    gp_wrap_ogl_sdl ogl_sdl(640,480,true);
    in_device_sdl input_sdl;

    img_loader.add_decoder(img_dec_png);

    app.main_loop(ogl_sdl, input_sdl);

    std::unique_ptr<gp_image_data> img = img_loader.load_image("/Users/bl/Downloads/Sortiert/Develop/Bildschirmfoto 2014-10-05 um 17.02.59.png");


    ogl_sdl.de_init();
#endif
#endif

    return 0;
}
