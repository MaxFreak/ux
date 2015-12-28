//
// Created by Bernd Lappas on 13.12.15.
//

#include "ux_resources.h"
#include "ux_types_and_defs.h"

#include <unordered_set>
#include <boost/filesystem.hpp>

namespace ux
{

const ux_string resources_dir = "resources";
const ux_string images_dir = "images";
const ux_string fonts_dir = "fonts";
const ux_string sounds_dir = "sounds";
const ux_string other_dir = "other";

const ux_resource empty_resource("", resource_type::empty);

using namespace boost::filesystem;

static path relative_to(path from, path to)
{
    // Start at the root path and while they are the same then do nothing then when they first
    // diverge take the remainder of the two path and replace the entire from path with ".."
    // segments.
    path::const_iterator fromIter = from.begin();
    path::const_iterator toIter = to.begin();

    // Loop through both
    while (fromIter != from.end() && toIter != to.end() && (*toIter) == (*fromIter))
    {
        ++toIter;
        ++fromIter;
    }

    path finalPath;
    while (fromIter != from.end())
    {
        finalPath /= "..";
        ++fromIter;
    }

    while (toIter != to.end())
    {
        finalPath /= *toIter;
        ++toIter;
    }

    return finalPath;
}

ux_resources::ux_resources()
{
    m_resource_type_path.push_back("images");
    m_resource_type_path.push_back("fonts");
    m_resource_type_path.push_back("sounds");
    m_resource_type_path.push_back("others");

    m_resource_type_skin.push_back(""); // default image skin
    m_resource_type_skin.push_back(""); // default font skin
    m_resource_type_skin.push_back(""); // default sound skin
    m_resource_type_skin.push_back(""); // default other skin
}

ux_resources::~ux_resources()
{
}

void ux_resources::setup_resources_paths(ux_string res_path)
{
    using namespace boost::filesystem;

    path resources_path(res_path);
    if (is_directory(resources_path))
    {
        for (const ux_string &res_type_path : m_resource_type_path)
        {
            path res_specific_path = resources_path / res_type_path;
            if(!exists(res_specific_path))
            {
                create_directory(res_specific_path);
            }
        }
    }
}

void ux_resources::load_resources_index(ux_string res_path)
{
    using namespace boost::filesystem;

    setup_resources_paths(res_path);

    path resources_path(res_path);
    if (is_directory(resources_path))
    {
        ux_uint dir_num = 0;
        for (const ux_string &res_type_path : m_resource_type_path)
        {
            path res_specific_path = resources_path / res_type_path;
            if(exists(res_specific_path))
            {
                load_resources_index_subpath(res_specific_path.generic_string(), static_cast<resource_type>(dir_num));
            }
            ++dir_num;
        }
    }
}

void ux_resources::load_resources_index_subpath(ux_string sub_path, resource_type res_type)
{
    using namespace boost::filesystem;

    path res_specific_path(sub_path);
    std::unordered_set<ux_string> res_set;

    for (directory_entry& dir_entry : directory_iterator(res_specific_path))
    {
        if (!is_directory(dir_entry))
        {
            res_set.emplace(dir_entry.path().filename().generic_string());
            m_resources_paths.push_back(ux_resource(dir_entry.path().filename().generic_string(), res_type));
        }
        else
        {
//            path rt  = relative_to(sub_path, dir_entry.path());
            for (directory_entry& skin_dir_entry : directory_iterator(dir_entry))
            {
                res_set.emplace(skin_dir_entry.path().filename().generic_string());
                m_resources_paths.push_back(ux_resource(dir_entry.path().filename().generic_string() + "/" + skin_dir_entry.path().filename().generic_string(), res_type));
            }
        }
    }
    for (const auto &p : res_set)
    {
        m_resource_ids.push_back(path_and_type(p, res_type));
    }
}

ux_uint ux_resources::add_resource(ux_string file_path, resource_type type_of_resource)
{
    m_resources_paths.push_back(ux_resource(file_path, type_of_resource));
    return m_resources_paths.size()-1;
}

void ux_resources::remove_resource(ux_string file_path)
{
    auto erase_cond = [&file_path](ux_resource &resource)
    {
        if (resource.get_file_path() == file_path)
            return true;
        return false;
    };

    m_resources_paths.erase(std::remove_if(m_resources_paths.begin(), m_resources_paths.end(), erase_cond), m_resources_paths.end());
}

ux_uint ux_resources::add_resource_id(ux_string file_path, resource_type type_of_resource)
{
    m_resource_ids.push_back(path_and_type(file_path, type_of_resource));
    return m_resource_ids.size()-1;
}

void ux_resources::remove_resource_id(ux_string file_path)
{
    auto erase_cond = [&file_path](path_and_type &resource)
    {
        if (resource.m_path == file_path)
            return true;
        return false;
    };

    m_resource_ids.erase(std::remove_if(m_resource_ids.begin(), m_resource_ids.end(), erase_cond), m_resource_ids.end());
}

const ux_resource &ux_resources::get_resource(ux_uint resource_id)
{
    // weitere Dereferenzierung fehlt.
    // d.h. eigener vektor, welcher dann auf den m_resources vektor zeigt
    // sollte verwendet werden (dieser fehlt noch)
//    if (resource_id < m_resource_ids.size())
//        return m_resource_ids[resource_id];

    return empty_resource;
}

} // namespace ux

