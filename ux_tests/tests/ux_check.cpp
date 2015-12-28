#include "gtest/gtest.h"
#include "ux_app.h"
#include "gp_wrap.h"

const char* dummy_args[] = { "/Users/bl/ClionProjects/ux/bin/Debug/run_ux"};

class ux_app_fixtures : public ::testing::Test
{
protected:
    virtual void  SetUp()
    {

    }

    virtual void TearDown()
    {

    }

    ux_app_fixtures() : Test()
    {
        int argc = 1;
//        char* args[] = {"/Users/bl/ClionProjects/ux/bin/Debug/run_ux"};

        app = new ux::ux_app(argc, (char**) dummy_args);
    }
    virtual ~ux_app_fixtures()
    {
        delete app;
    }

    ux::ux_app *app;
};

TEST_F(ux_app_fixtures, get_exe_file_path)
{
    ux_string args = dummy_args[0];
    ux_string file_path = app->get_exe_file_path();

    EXPECT_EQ(args, file_path);
}

TEST_F(ux_app_fixtures, set_exe_file_path)
{
    const ux_string exe_path = "Dies ist ein test";
    app->set_exe_file_path(exe_path);
    ux_string file_path = app->get_exe_file_path();

    EXPECT_EQ(exe_path, file_path);
}

TEST_F(ux_app_fixtures, get_exe_path)
{
    ux_string exe_path = "/Users/bl/ClionProjects/ux/bin/Debug";
    ux_string get_path = app->get_exe_path();

    EXPECT_EQ(exe_path, get_path);
}

TEST_F(ux_app_fixtures, set_exe_path)
{
    const ux_string exe_path = "Dies ist ein test";
    app->set_exe_path(exe_path);
    ux_string get_path = app->get_exe_path();

    EXPECT_EQ(exe_path, get_path);
}

TEST_F(ux_app_fixtures, get_resources_path)
{
    ux_string resource_test_path = "/Users/bl/ClionProjects/ux/bin/Debug/resources";
    ux_string res_path = app->get_resources_path();

    EXPECT_EQ(resource_test_path, res_path);
}

TEST_F(ux_app_fixtures, set_resources_path)
{
    const ux_string resource_test_path = "Dies ist ein test";
    app->set_resources_path(resource_test_path);
    ux_string res_path = app->get_resources_path();

    EXPECT_EQ(resource_test_path, res_path);
}

TEST_F(ux_app_fixtures, from_rgb)
{
    ux_uint black = ux::gp::from_rgb(0 , 0 , 0);
    ux_uint white = ux::gp::from_rgb(0xff , 0xff , 0xff);
    ux_uint red = ux::gp::from_rgb(0xff , 0 , 0);
    ux_uint green = ux::gp::from_rgb(0 , 0xff , 0);
    ux_uint blue = ux::gp::from_rgb(0 , 0 , 0xff);
    ux_uint midgrey = ux::gp::from_rgb(0x80 , 0x80 , 0x80);

    EXPECT_EQ(0x000000, black);
    EXPECT_EQ(0xffffff, white);
    EXPECT_EQ(0xff0000, red);
    EXPECT_EQ(0x00ff00, green);
    EXPECT_EQ(0x0000ff, blue);
    EXPECT_EQ(0x808080, midgrey);

    ux_uint minus_one_white = ux::gp::from_rgb(-1 , -1 , -1);
    EXPECT_EQ(0xffffff, minus_one_white);
}

TEST_F(ux_app_fixtures, from_argb)
{
    ux_uint trans_black = ux::gp::from_argb(0, 0 , 0 , 0);
    ux_uint trans_white = ux::gp::from_argb(0, 0xff , 0xff , 0xff);
    ux_uint trans_red = ux::gp::from_argb(0, 0xff , 0 , 0);
    ux_uint trans_green = ux::gp::from_argb(0, 0 , 0xff , 0);
    ux_uint trans_blue = ux::gp::from_argb(0, 0 , 0 , 0xff);
    ux_uint trans_midgrey = ux::gp::from_argb(0, 0x80 , 0x80 , 0x80);

    ux_uint black = ux::gp::from_argb(0xff, 0 , 0 , 0);
    ux_uint white = ux::gp::from_argb(0xff, 0xff , 0xff , 0xff);
    ux_uint red = ux::gp::from_argb(0xff, 0xff , 0 , 0);
    ux_uint green = ux::gp::from_argb(0xff, 0 , 0xff , 0);
    ux_uint blue = ux::gp::from_argb(0xff, 0 , 0 , 0xff);
    ux_uint midgrey = ux::gp::from_argb(0xff, 0x80 , 0x80 , 0x80);

    EXPECT_EQ(0x000000, trans_black);
    EXPECT_EQ(0xffffff, trans_white);
    EXPECT_EQ(0xff0000, trans_red);
    EXPECT_EQ(0x00ff00, trans_green);
    EXPECT_EQ(0x0000ff, trans_blue);
    EXPECT_EQ(0x808080, trans_midgrey);

    EXPECT_EQ(0xff000000, black);
    EXPECT_EQ(0xffffffff, white);
    EXPECT_EQ(0xffff0000, red);
    EXPECT_EQ(0xff00ff00, green);
    EXPECT_EQ(0xff0000ff, blue);
    EXPECT_EQ(0xff808080, midgrey);

    ux_uint minus_one_white = ux::gp::from_argb(-1, -1 , -1 , -1);
    EXPECT_EQ(0xffffffff, minus_one_white);
}

TEST_F(ux_app_fixtures, get_a_from_argb)
{
    ux_uint trans_black = ux::gp::from_argb(0, 0 , 0 , 0);
    ux_uint trans_white = ux::gp::from_argb(0, 0xff , 0xff , 0xff);
    ux_uint trans_red = ux::gp::from_argb(0, 0xff , 0 , 0);
    ux_uint trans_green = ux::gp::from_argb(0, 0 , 0xff , 0);
    ux_uint trans_blue = ux::gp::from_argb(0, 0 , 0 , 0xff);
    ux_uint trans_midgrey = ux::gp::from_argb(0, 0x80 , 0x80 , 0x80);

    ux_uint black = ux::gp::from_argb(0xff, 0 , 0 , 0);
    ux_uint white = ux::gp::from_argb(0xff, 0xff , 0xff , 0xff);
    ux_uint red = ux::gp::from_argb(0xff, 0xff , 0 , 0);
    ux_uint green = ux::gp::from_argb(0xff, 0 , 0xff , 0);
    ux_uint blue = ux::gp::from_argb(0xff, 0 , 0 , 0xff);
    ux_uint midgrey = ux::gp::from_argb(0xff, 0x80 , 0x80 , 0x80);
    ux_uint halfgrey = ux::gp::from_argb(0x80, 0x80 , 0x80 , 0x80);

    ux_ubyte a_trans_black = ux::gp::get_a_from_argb(trans_black);
    ux_ubyte a_trans_white = ux::gp::get_a_from_argb(trans_white);
    ux_ubyte a_trans_red = ux::gp::get_a_from_argb(trans_red);
    ux_ubyte a_trans_green = ux::gp::get_a_from_argb(trans_green);
    ux_ubyte a_trans_blue = ux::gp::get_a_from_argb(trans_blue);
    ux_ubyte a_trans_midgrey = ux::gp::get_a_from_argb(trans_midgrey);

    ux_ubyte a_black = ux::gp::get_a_from_argb(black);
    ux_ubyte a_white = ux::gp::get_a_from_argb(white);
    ux_ubyte a_red = ux::gp::get_a_from_argb(red);
    ux_ubyte a_green = ux::gp::get_a_from_argb(green);
    ux_ubyte a_blue = ux::gp::get_a_from_argb(blue);
    ux_ubyte a_midgrey = ux::gp::get_a_from_argb(midgrey);
    ux_ubyte a_halfgrey = ux::gp::get_a_from_argb(halfgrey);

    EXPECT_EQ(0x00, a_trans_black);
    EXPECT_EQ(0x00, a_trans_white);
    EXPECT_EQ(0x00, a_trans_red);
    EXPECT_EQ(0x00, a_trans_green);
    EXPECT_EQ(0x00, a_trans_blue);
    EXPECT_EQ(0x00, a_trans_midgrey);

    EXPECT_EQ(0xff, a_black);
    EXPECT_EQ(0xff, a_white);
    EXPECT_EQ(0xff, a_red);
    EXPECT_EQ(0xff, a_green);
    EXPECT_EQ(0xff, a_blue);
    EXPECT_EQ(0xff, a_midgrey);
    EXPECT_EQ(0x80, a_halfgrey);
}

TEST_F(ux_app_fixtures, get_r_from_argb)
{
    ux_uint trans_black = ux::gp::from_argb(0, 0 , 0 , 0);
    ux_uint trans_white = ux::gp::from_argb(0, 0xff , 0xff , 0xff);
    ux_uint trans_red = ux::gp::from_argb(0, 0xff , 0 , 0);
    ux_uint trans_green = ux::gp::from_argb(0, 0 , 0xff , 0);
    ux_uint trans_blue = ux::gp::from_argb(0, 0 , 0 , 0xff);
    ux_uint trans_midgrey = ux::gp::from_argb(0, 0x80 , 0x80 , 0x80);

    ux_uint black = ux::gp::from_argb(0xff, 0 , 0 , 0);
    ux_uint white = ux::gp::from_argb(0xff, 0xff , 0xff , 0xff);
    ux_uint red = ux::gp::from_argb(0xff, 0xff , 0 , 0);
    ux_uint green = ux::gp::from_argb(0xff, 0 , 0xff , 0);
    ux_uint blue = ux::gp::from_argb(0xff, 0 , 0 , 0xff);
    ux_uint midgrey = ux::gp::from_argb(0xff, 0x80 , 0x80 , 0x80);
    ux_uint halfgrey = ux::gp::from_argb(0x80, 0x80 , 0x80 , 0x80);

    ux_ubyte a_trans_black = ux::gp::get_r_from_argb(trans_black);
    ux_ubyte a_trans_white = ux::gp::get_r_from_argb(trans_white);
    ux_ubyte a_trans_red = ux::gp::get_r_from_argb(trans_red);
    ux_ubyte a_trans_green = ux::gp::get_r_from_argb(trans_green);
    ux_ubyte a_trans_blue = ux::gp::get_r_from_argb(trans_blue);
    ux_ubyte a_trans_midgrey = ux::gp::get_r_from_argb(trans_midgrey);

    ux_ubyte a_black = ux::gp::get_r_from_argb(black);
    ux_ubyte a_white = ux::gp::get_r_from_argb(white);
    ux_ubyte a_red = ux::gp::get_r_from_argb(red);
    ux_ubyte a_green = ux::gp::get_r_from_argb(green);
    ux_ubyte a_blue = ux::gp::get_r_from_argb(blue);
    ux_ubyte a_midgrey = ux::gp::get_r_from_argb(midgrey);
    ux_ubyte a_halfgrey = ux::gp::get_r_from_argb(halfgrey);

    EXPECT_EQ(0x00, a_trans_black);
    EXPECT_EQ(0xff, a_trans_white);
    EXPECT_EQ(0xff, a_trans_red);
    EXPECT_EQ(0x00, a_trans_green);
    EXPECT_EQ(0x00, a_trans_blue);
    EXPECT_EQ(0x80, a_trans_midgrey);

    EXPECT_EQ(0x00, a_black);
    EXPECT_EQ(0xff, a_white);
    EXPECT_EQ(0xff, a_red);
    EXPECT_EQ(0x00, a_green);
    EXPECT_EQ(0x00, a_blue);
    EXPECT_EQ(0x80, a_midgrey);
    EXPECT_EQ(0x80, a_halfgrey);
}

TEST_F(ux_app_fixtures, get_g_from_argb)
{
    ux_uint trans_black = ux::gp::from_argb(0, 0 , 0 , 0);
    ux_uint trans_white = ux::gp::from_argb(0, 0xff , 0xff , 0xff);
    ux_uint trans_red = ux::gp::from_argb(0, 0xff , 0 , 0);
    ux_uint trans_green = ux::gp::from_argb(0, 0 , 0xff , 0);
    ux_uint trans_blue = ux::gp::from_argb(0, 0 , 0 , 0xff);
    ux_uint trans_midgrey = ux::gp::from_argb(0, 0x80 , 0x80 , 0x80);

    ux_uint black = ux::gp::from_argb(0xff, 0 , 0 , 0);
    ux_uint white = ux::gp::from_argb(0xff, 0xff , 0xff , 0xff);
    ux_uint red = ux::gp::from_argb(0xff, 0xff , 0 , 0);
    ux_uint green = ux::gp::from_argb(0xff, 0 , 0xff , 0);
    ux_uint blue = ux::gp::from_argb(0xff, 0 , 0 , 0xff);
    ux_uint midgrey = ux::gp::from_argb(0xff, 0x80 , 0x80 , 0x80);
    ux_uint halfgrey = ux::gp::from_argb(0x80, 0x80 , 0x80 , 0x80);

    ux_ubyte a_trans_black = ux::gp::get_g_from_argb(trans_black);
    ux_ubyte a_trans_white = ux::gp::get_g_from_argb(trans_white);
    ux_ubyte a_trans_red = ux::gp::get_g_from_argb(trans_red);
    ux_ubyte a_trans_green = ux::gp::get_g_from_argb(trans_green);
    ux_ubyte a_trans_blue = ux::gp::get_g_from_argb(trans_blue);
    ux_ubyte a_trans_midgrey = ux::gp::get_g_from_argb(trans_midgrey);

    ux_ubyte a_black = ux::gp::get_g_from_argb(black);
    ux_ubyte a_white = ux::gp::get_g_from_argb(white);
    ux_ubyte a_red = ux::gp::get_g_from_argb(red);
    ux_ubyte a_green = ux::gp::get_g_from_argb(green);
    ux_ubyte a_blue = ux::gp::get_g_from_argb(blue);
    ux_ubyte a_midgrey = ux::gp::get_g_from_argb(midgrey);
    ux_ubyte a_halfgrey = ux::gp::get_g_from_argb(halfgrey);

    EXPECT_EQ(0x00, a_trans_black);
    EXPECT_EQ(0xff, a_trans_white);
    EXPECT_EQ(0x00, a_trans_red);
    EXPECT_EQ(0xff, a_trans_green);
    EXPECT_EQ(0x00, a_trans_blue);
    EXPECT_EQ(0x80, a_trans_midgrey);

    EXPECT_EQ(0x00, a_black);
    EXPECT_EQ(0xff, a_white);
    EXPECT_EQ(0x00, a_red);
    EXPECT_EQ(0xff, a_green);
    EXPECT_EQ(0x00, a_blue);
    EXPECT_EQ(0x80, a_midgrey);
    EXPECT_EQ(0x80, a_halfgrey);
}

TEST_F(ux_app_fixtures, get_b_from_argb)
{
    ux_uint trans_black = ux::gp::from_argb(0, 0 , 0 , 0);
    ux_uint trans_white = ux::gp::from_argb(0, 0xff , 0xff , 0xff);
    ux_uint trans_red = ux::gp::from_argb(0, 0xff , 0 , 0);
    ux_uint trans_green = ux::gp::from_argb(0, 0 , 0xff , 0);
    ux_uint trans_blue = ux::gp::from_argb(0, 0 , 0 , 0xff);
    ux_uint trans_midgrey = ux::gp::from_argb(0, 0x80 , 0x80 , 0x80);

    ux_uint black = ux::gp::from_argb(0xff, 0 , 0 , 0);
    ux_uint white = ux::gp::from_argb(0xff, 0xff , 0xff , 0xff);
    ux_uint red = ux::gp::from_argb(0xff, 0xff , 0 , 0);
    ux_uint green = ux::gp::from_argb(0xff, 0 , 0xff , 0);
    ux_uint blue = ux::gp::from_argb(0xff, 0 , 0 , 0xff);
    ux_uint midgrey = ux::gp::from_argb(0xff, 0x80 , 0x80 , 0x80);
    ux_uint halfgrey = ux::gp::from_argb(0x80, 0x80 , 0x80 , 0x80);

    ux_ubyte a_trans_black = ux::gp::get_b_from_argb(trans_black);
    ux_ubyte a_trans_white = ux::gp::get_b_from_argb(trans_white);
    ux_ubyte a_trans_red = ux::gp::get_b_from_argb(trans_red);
    ux_ubyte a_trans_green = ux::gp::get_b_from_argb(trans_green);
    ux_ubyte a_trans_blue = ux::gp::get_b_from_argb(trans_blue);
    ux_ubyte a_trans_midgrey = ux::gp::get_b_from_argb(trans_midgrey);

    ux_ubyte a_black = ux::gp::get_b_from_argb(black);
    ux_ubyte a_white = ux::gp::get_b_from_argb(white);
    ux_ubyte a_red = ux::gp::get_b_from_argb(red);
    ux_ubyte a_green = ux::gp::get_b_from_argb(green);
    ux_ubyte a_blue = ux::gp::get_b_from_argb(blue);
    ux_ubyte a_midgrey = ux::gp::get_b_from_argb(midgrey);
    ux_ubyte a_halfgrey = ux::gp::get_b_from_argb(halfgrey);

    EXPECT_EQ(0x00, a_trans_black);
    EXPECT_EQ(0xff, a_trans_white);
    EXPECT_EQ(0x00, a_trans_red);
    EXPECT_EQ(0x00, a_trans_green);
    EXPECT_EQ(0xff, a_trans_blue);
    EXPECT_EQ(0x80, a_trans_midgrey);

    EXPECT_EQ(0x00, a_black);
    EXPECT_EQ(0xff, a_white);
    EXPECT_EQ(0x00, a_red);
    EXPECT_EQ(0x00, a_green);
    EXPECT_EQ(0xff, a_blue);
    EXPECT_EQ(0x80, a_midgrey);
    EXPECT_EQ(0x80, a_halfgrey);
}


