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
    using namespace boost::filesystem;

    ux_string exe_file_path = args[0];
    ux_string exe_path = initial_path().generic_string();

    path cur_path = current_path();
    ux_string cws = cur_path.generic_string();

    cout << "Current working directory: " << cws << "\n\n";
        path p ("../..");

        try
        {
            if (exists(p))
            {
                if (is_directory(p))
                {
                    std::vector<std::string> v;

                    for (auto&& x : directory_iterator(p))
                    {
                        v.push_back(x.path().string());
                        path pa(x);
                        cout << "    " << x << " " << is_directory(pa) << " " << pa.relative_path() << " " << pa.parent_path() << '\n';
                    }
//                    v.push_back(x.path().filename().string());

                    std::sort(v.begin(), v.end());

                    for (auto&& x : v)
                    {
                        cout << "    " << x << '\n';
                    }

                }
                else
                    cout << p << " exists, but is not a regular file or directory\n";
            }
            else
                cout << p << " does not exist\n";
        }
        catch (const filesystem_error& ex)
        {
            cout << ex.what() << '\n';
        }


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
//    glfw_main();

    cout << "Hello, World!\n" ;

    print_all(cout, "Hello", " World", '!', 0 ,'\n');

    ui_history h(1);

    current(h).emplace_back("Hello");
    current(h).emplace_back("World");
    current(h).emplace_back("1");
    current(h).emplace_back("2");
    current(h).emplace_back("3");
    current(h).emplace_back("4");
    current(h).emplace_back("5");
    current(h).emplace_back("6");
    current(h).emplace_back("7");
    current(h).emplace_back("8");
    current(h).emplace_back("9");
    current(h).emplace_back("10");
    current(h).emplace_back(my_class_t());

    current(h).operator[](1).test();
    current(h)[1].test();
    ui_object uio = current(h)[1];
    uio.test();
    auto &uior = current(h)[1];
    uior.test();
//??    boost::uuids::uuid ot = uior.get_tag();

//    boost::uuids::uuid ot = current(h).m_Childs[0].GetTag();

//    commit(h);
//    current(h).emplace_back(current(h));
//    commit(h);
//    current(h).emplace_back(current(h));

    print_to(h, cout);
    draw(h, cout);
    cout.flush();

    return 0;
}
