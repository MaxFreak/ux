//
// Created by Bernd Lappas on 13.12.15.
//

#ifndef UX_UX_RESOURCE_H
#define UX_UX_RESOURCE_H

#include "ux_types_and_defs.h"

namespace ux
{

// a path is just the path without filename
// a file_path is a path including filename (with extension if there is one)
// a file_name is just the filename (with extension if there is one)
// a folder or directory is always given as path

enum class resource_type
{
    empty, image, font, sound, other
};

class ux_resource
{
    struct data {};
    struct image_data: data
    {

    };
    struct font_data: data
    {

    };
    struct sound_data: data
    {

    };
    struct other_data: data
    {

    };
public:
    ux_resource(ux_string file_path, resource_type m_resource_type);

    virtual ~ux_resource();

    virtual void load() {
        switch (m_resource_type)
        {
            case resource_type::empty:break;
            case resource_type::image:
                m_resource_content = new image_data();
                break;
            case resource_type::font:
                m_resource_content = new font_data();
                break;
            case resource_type::sound:
                m_resource_content = new sound_data();
                break;
            case resource_type::other:
                m_resource_content = new other_data();
                break;
        }
    };
    virtual void unload(){
        delete(m_resource_content);
    };

    ux_bool is_empty() const {return m_resource_type == resource_type::empty; };
    const resource_type & get_resource_type() const;

    void set_resource_type(const resource_type &resource_type);

    const ux_string & get_file_path() const;

    void set_file_path(const ux_string &file_path);

    const data& get_resource_content() const
    {
        return *m_resource_content;
    }

private:
    ux_string file_path;
    resource_type m_resource_type;
    data *m_resource_content;
};

} // namespace ux

#endif //UX_UX_RESOURCE_H
