//
// Created by Bernd Lappas on 13.12.15.
//

#ifndef UX_UX_RESOURCE_H
#define UX_UX_RESOURCE_H

#include "ux_types_and_defs.h"

namespace ux
{

// a path is just the path without filename
// a m_file_path is a path including filename (with extension if there is one)
// a file_name is just the filename (with extension if there is one)
// a folder or directory is always given as path

enum class resource_type
{
    empty = -1, image, font, sound, other
};

class ux_resource
{
    struct data
    {
        virtual data* clone() const = 0;
        virtual ~data(){};
    };

    struct image_data: data
    {
        data* clone() const { return new image_data(); }
    };
    struct font_data: data
    {
        data* clone() const { return new font_data(); }
    };
    struct sound_data: data
    {
        data* clone() const { return new sound_data(); }
    };
    struct other_data: data
    {
        data* clone() const { return new other_data(); }
    };
public:
    ux_resource(ux_string file_path, resource_type m_resource_type);

    virtual ~ux_resource();

    // copy constructor
    ux_resource(const ux_resource &org) : m_file_path(org.m_file_path), m_resource_type(org.m_resource_type),
                                          m_resource_content(org.m_resource_content ? org.m_resource_content->clone() : nullptr)
    {
    }

    // copy assignment operator
    ux_resource &operator=(ux_resource org)
    {
        swap(*this, org);
        return *this;
    }

    // move constructor
    ux_resource(ux_resource &&temp)
    {
        swap(*this, temp);
    }

    friend void swap(ux_resource &first, ux_resource &second) noexcept
    {
        using std::swap;
        swap(first.m_file_path, second.m_file_path);
        swap(first.m_resource_type, second.m_resource_type);
        swap(first.m_resource_content, second.m_resource_content);
    }

    virtual data *factory()
    {
        data *resource_content = nullptr;

        switch (m_resource_type)
        {
            case resource_type::empty:break;
            case resource_type::image:
                resource_content = new image_data();
                break;
            case resource_type::font:
                resource_content = new font_data();
                break;
            case resource_type::sound:
                resource_content = new sound_data();
                break;
            case resource_type::other:
                resource_content = new other_data();
                break;
        }
        return resource_content;
    };

    virtual void load()
    {
        factory();
    };

    virtual void unload()
    {
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
    ux_string m_file_path;
    resource_type m_resource_type;
    data *m_resource_content;
};

} // namespace ux

#endif //UX_UX_RESOURCE_H
