//
// Created by Bernd Lappas on 13.12.15.
//

#include "ux_resource.h"

namespace ux
{

ux_resource::ux_resource(ux_string file_path, resource_type m_resource_type) : m_resource_type(m_resource_type),
                                                                               file_path(file_path),
                                                                               m_resource_content(nullptr)
{

}

ux_resource::~ux_resource()
{
    delete m_resource_content;
}

const resource_type &ux_resource::get_resource_type() const
{
    return m_resource_type;
}

void ux_resource::set_resource_type(const resource_type &resource_type)
{
    m_resource_type = resource_type;
}

const ux_string &ux_resource::get_file_path() const
{
    return file_path;
}

void ux_resource::set_file_path(const ux_string &file_path)
{
    ux_resource::file_path = file_path;
}

} // namespace ux
