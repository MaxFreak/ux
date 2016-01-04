#include "gtest/gtest.h"
#include "gmock/gmock.h"  // Brings in Google Mock.
#include "ux_resource.h"
#include "ux_resources.h"
#include "gsl.h"

#include <boost/filesystem.hpp>
#include <iostream>

class mock_ux_resource : public ux::ux_resource
{
public:
    mock_ux_resource():ux::ux_resource("", ux::resource_type::empty) {}
    mock_ux_resource(ux_string file_path, ux::resource_type resource_type):ux::ux_resource(file_path, resource_type) {}
    // Add the following two lines to the mock class.
    MOCK_METHOD0(destruct, void());
    virtual ~mock_ux_resource() { destruct(); }
};

class access_ux_resources : public ux::ux_resources
{
public:
    access_ux_resources():ux::ux_resources() {  }
    virtual ~access_ux_resources() {  }

    const std::vector<path_and_type> &get_resource_ids() const { return m_resource_ids;}
    const std::vector<ux::ux_resource> &get_resources_path() const { return m_resources_paths;}
    const std::vector<ux_string> &get_resource_type_path() const { return m_resources_type_path;}
    const std::vector<ux_string> &get_resource_type_skin() const { return m_resources_type_skin;}
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

TEST_F(ux_resources_fixtures, copyconstructor_ux_resource)
{
    {
        ux::ux_resource res(ux::ux_resource("", ux::resource_type::image));
        auto dat = res.get_resource_data();

        EXPECT_EQ(0, dat.size());
        EXPECT_EQ(nullptr, dat.data());
        EXPECT_EQ(nullptr, res.get_resource_content());
        res.load();
        auto dat2 = res.get_resource_data();
        EXPECT_EQ(1, dat2.size());
        EXPECT_NE(nullptr, dat2.data());
        EXPECT_NE(nullptr, res.get_resource_content());

        ux::ux_resource copy(res);
        ux::ux_resource copy2 = res;
    }
}

TEST_F(ux_resources_fixtures, copyassignment_ux_resource)
{
    {
        ux::ux_resource res(ux::ux_resource("", ux::resource_type::image));
        ux::ux_resource assign("", ux::resource_type::empty);
        auto datres = res.get_resource_data();
        auto datassign = res.get_resource_data();

        assign = res;

        EXPECT_EQ(0, datassign.size());
        EXPECT_EQ(nullptr, datassign.data());
        EXPECT_EQ(ux::resource_type::image, assign.get_resource_type());
        EXPECT_EQ(nullptr, assign.get_resource_content());

        res.load();
        assign = res;
        gsl::span<ux_ubyte> datres2 = res.get_resource_data();
        gsl::span<ux_ubyte>  datassign2 = assign.get_resource_data();

        EXPECT_EQ(1, datassign2.size());
        EXPECT_NE(nullptr, datassign2.data());
        EXPECT_EQ(ux::resource_type::image, assign.get_resource_type());
        EXPECT_NE(nullptr, assign.get_resource_content());

        auto datassign3 = assign.get_resource_data();
        EXPECT_EQ(1, datassign3.size());
        EXPECT_NE(nullptr, datassign3.data());
        EXPECT_NE(nullptr, assign.get_resource_content());
        EXPECT_NE(res.get_resource_content(), assign.get_resource_content());

        auto dat = datassign3.data();
        EXPECT_EQ(12, dat[0]);
    }
}

TEST_F(ux_resources_fixtures, constructor_ux_resources)
{
    ux::ux_resources resources;
    access_ux_resources access_resources;

    auto ids_vector = access_resources.get_resource_ids();
    auto paths_vector = access_resources.get_resources_path();
    auto type_vector = access_resources.get_resource_type_path();
    auto skin_vector = access_resources.get_resource_type_skin();

    EXPECT_EQ(0, ids_vector.size());
    EXPECT_EQ(0, paths_vector.size());
    EXPECT_EQ(4, type_vector.size());
    EXPECT_EQ(4, skin_vector.size());

    EXPECT_EQ("images", type_vector[0]);
    EXPECT_EQ("fonts", type_vector[1]);
    EXPECT_EQ("sounds", type_vector[2]);
    EXPECT_EQ("others", type_vector[3]);

    EXPECT_EQ("", skin_vector[0]);
    EXPECT_EQ("", skin_vector[1]);
    EXPECT_EQ("", skin_vector[2]);
    EXPECT_EQ("", skin_vector[3]);
}

TEST_F(ux_resources_fixtures, load_resources_index)
{
   // ux::ux_resources resources;
    access_ux_resources access_resources;

    using namespace boost::filesystem;
    path test_resources_path = initial_path() / "test_resources";

    access_resources.load_resources_index(test_resources_path.generic_string());

    auto ids_vector = access_resources.get_resource_ids();
    auto paths_vector = access_resources.get_resources_path();
    auto type_vector = access_resources.get_resource_type_path();
    auto skin_vector = access_resources.get_resource_type_skin();

    int i = 0;
    std::cout << std::setw(  2 ) << std::dec;
    for (const auto &p : ids_vector)
    {
        std::cout << std::setw(  2 ) << i++ << " " << p.m_path << " " << type_vector[static_cast<int>(p.m_type)] << std::endl;
    }

    EXPECT_EQ(25, ids_vector.size());
    EXPECT_EQ(31, paths_vector.size());
    EXPECT_EQ(4, type_vector.size());
    EXPECT_EQ(4, skin_vector.size());

    EXPECT_EQ("images", type_vector[0]);
    EXPECT_EQ("fonts", type_vector[1]);
    EXPECT_EQ("sounds", type_vector[2]);
    EXPECT_EQ("others", type_vector[3]);

    EXPECT_EQ("", skin_vector[0]);
    EXPECT_EQ("", skin_vector[1]);
    EXPECT_EQ("", skin_vector[2]);
    EXPECT_EQ("", skin_vector[3]);
}


