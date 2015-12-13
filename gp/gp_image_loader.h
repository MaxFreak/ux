//
// Created by Bernd Lappas on 09.12.15.
//

#ifndef UX_GP_IMAGE_LOADER_H
#define UX_GP_IMAGE_LOADER_H

#include <vector>

#include "ux_types_and_defs.h"

#include "gp_image_data.h"
#include "gp_image_decoder.h"

class gp_image_loader
{
public:
    gp_image_loader() {};

    ~gp_image_loader() {};

    void init();

    /** Emptys the list and deletes all decoder pointers. Should not be called from user code. */
    void de_init();

    std::unique_ptr<gp_image_data> load_image(const ux_string &file_path);

    void add_decoder(gp_image_decoder &image_decoder);
private:
    std::vector<std::reference_wrapper<gp_image_decoder>> m_decoder_vector;
};

#endif //UX_GP_IMAGE_LOADER_H
