//
// Created by Bernd Lappas on 09.12.15.
//

#include "gp_image_data.h"

gp_image_data::gp_image_data(): m_width(0), m_height(0), m_image_type(RGBA_8888), m_filler_byte(0xff), m_image_data(
    nullptr), m_start_top_left(true), m_delete_data(true)
{
}

gp_image_data::~gp_image_data()
{
    if (m_delete_data)
    {
        delete[] m_image_data;
        m_image_data = NULL;
    }
}

ux_bool gp_image_data::has_alpha() const
{
    if (m_image_type == ABGR_4444 || m_image_type == BGRA_4444 || m_image_type == ARGB_8888 ||
        m_image_type == ABGR_8888 || m_image_type == RGBA_8888 || m_image_type == BGRA_8888)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void gp_image_data::delete_data()
{
    delete[] m_image_data;
    m_image_data = NULL;
}
