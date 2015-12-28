#include "gtest/gtest.h"
#include "gmock/gmock.h"  // Brings in Google Mock.
#include "ux_resource.h"
#include "ux_resources.h"

class mock_ux_resource : public ux::ux_resource
{
public:
    mock_ux_resource():ux::ux_resource("", ux::resource_type::empty) {}
    // Add the following two lines to the mock class.
    MOCK_METHOD0(destruct, void());
    virtual ~mock_ux_resource() { destruct(); }
};

class ux_resources_fixtures : public ::testing::Test
{
protected:
    virtual void  SetUp()
    {

    }

    virtual void TearDown()
    {

    }

    ux_resources_fixtures() : Test()
    {
//        int argc = 1;
//        app = new ux::ux_app(argc, (char**) dummy_args);
    }
    virtual ~ux_resources_fixtures()
    {
//        delete app;
    }

    //ux::ux_app *app;
};

TEST_F(ux_resources_fixtures, constructor_ux_resource)
{
    ux::ux_resource res("", ux::resource_type::empty);

    EXPECT_EQ("", res.get_file_path());
    EXPECT_EQ(true, res.is_empty());
}

TEST_F(ux_resources_fixtures, destructor_ux_resource)
{
    mock_ux_resource* res = new mock_ux_resource;
    {
        EXPECT_CALL(*res, destruct());
        delete(res);
    }
    ::testing::Mock::VerifyAndClearExpectations(res);
}

TEST_F(ux_resources_fixtures, get_resource_type)
{
    ux::ux_resource res1("", ux::resource_type::empty);
    ux::ux_resource res2("", ux::resource_type::image);
    ux::ux_resource res3("", ux::resource_type::font);
    ux::ux_resource res4("", ux::resource_type::sound);
    ux::ux_resource res5("", ux::resource_type::other);
    ux::ux_resource res6("", static_cast<ux::resource_type>(28)); // to be checked?

    EXPECT_EQ(ux::resource_type::empty, res1.get_resource_type());
    EXPECT_EQ(true, res1.is_empty());
    EXPECT_EQ(ux::resource_type::image, res2.get_resource_type());
    EXPECT_EQ(ux::resource_type::font, res3.get_resource_type());
    EXPECT_EQ(ux::resource_type::sound, res4.get_resource_type());
    EXPECT_EQ(ux::resource_type::other, res5.get_resource_type());
    // to be checked?    EXPECT_EQ(ux::resource_type::other, res6.get_resource_type());
}

TEST_F(ux_resources_fixtures, set_resource_type)
{
    ux::ux_resource res("", ux::resource_type::empty);

    res.set_resource_type(ux::resource_type::other);
    EXPECT_EQ(ux::resource_type::other, res.get_resource_type());

    res.set_resource_type(ux::resource_type::sound);
    EXPECT_EQ(ux::resource_type::sound, res.get_resource_type());

    res.set_resource_type(ux::resource_type::font);
    EXPECT_EQ(ux::resource_type::font, res.get_resource_type());

    res.set_resource_type(ux::resource_type::image);
    EXPECT_EQ(ux::resource_type::image, res.get_resource_type());

    res.set_resource_type(ux::resource_type::empty);
    EXPECT_EQ(ux::resource_type::empty, res.get_resource_type());
}

