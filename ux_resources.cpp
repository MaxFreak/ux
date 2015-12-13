//
// Created by Bernd Lappas on 13.12.15.
//

#include "ux_resources.h"
#include "ux_types_and_defs.h"

namespace ux
{

const ux_string resources_dir = "resources";
const ux_string images_dir = "images";
const ux_string fonts_dir = "fonts";
const ux_string sounds_dir = "sounds";
const ux_string other_dir = "other";

const ux_resource empty_resource("", resource_type::empty);

ux_resources::ux_resources()
{

}

ux_resources::~ux_resources()
{

}

ux_uint ux_resources::add_resource(ux_string file_path, resource_type type_of_resource)
{
    m_resources.push_back(ux_resource(file_path, type_of_resource));
    return m_resources.size()-1;
}

void ux_resources::remove_resource(ux_string file_path)
{
    auto erase_cond = [&file_path](ux_resource &resource)
    {
        if (resource.get_file_path() == file_path)
            return true;
        return false;
    };

    m_resources.erase(std::remove_if(m_resources.begin(), m_resources.end(), erase_cond), m_resources.end());
}

const ux_resource &ux_resources::get_resource(ux_uint resource_id)
{
    if (resource_id < m_resources.size())
        return m_resources[resource_id];

    return empty_resource;
}
} // namespace ux

