//
// Created by Bernd Lappas on 13.12.15.
//

#ifndef UX_UX_RESOURCES_H
#define UX_UX_RESOURCES_H

#include <vector>

#include "ux_types_and_defs.h"
#include "ux_resource.h"

namespace ux
{


class ux_resources
{

public:
    ux_resources();

    virtual ~ux_resources();

    ux_uint add_resource(ux_string file_path, resource_type type_of_resource);
    const ux_resource& get_resource(ux_uint resource_id);

    void remove_resource(ux_string file_path);
private:
    std::vector<ux_resource> m_resources;
};

} // namespace ux

#endif //UX_UX_RESOURCES_H
