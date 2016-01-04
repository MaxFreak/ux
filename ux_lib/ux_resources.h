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

    void load_resources_index(ux_string path) noexcept(false);
    const ux_resource& get_resource(ux_uint resource_id) noexcept(false);
protected:
    struct path_and_type
    {
        path_and_type(ux_string file_path, resource_type res_type) :m_path(file_path), m_type(res_type) {};
        ux_string       m_path;
        resource_type   m_type;
    };

    void load_resources_index_subpath(ux_string sub_path, resource_type res_type) noexcept(false);
    void setup_resources_paths(ux_string path) noexcept(false);

    ux_uint add_resource(ux_string file_path, resource_type type_of_resource) noexcept(false);
    void remove_resource(ux_string file_path) noexcept(false);
    ux_uint add_resource_id(ux_string file_path, resource_type type_of_resource) noexcept(false);
    void remove_resource_id(ux_string file_path) noexcept(false);

    std::vector<path_and_type> m_resource_ids;
    std::vector<ux_resource> m_resources_paths;
    std::vector<ux_string> m_resources_type_path;
    std::vector<ux_string> m_resources_type_skin; // current skin of resource type, empty for default skin
};

} // namespace ux

#endif //UX_UX_RESOURCES_H
