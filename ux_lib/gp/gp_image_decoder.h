//
// Created by Bernd Lappas on 09.12.15.
//

#ifndef UX_GP_IMAGE_DECODER_H
#define UX_GP_IMAGE_DECODER_H

#include "ux_types_and_defs.h"
#include "gp_image_data.h"

class gp_image_decoder
{
public:
    gp_image_decoder() {};
    virtual ~gp_image_decoder() {};
    virtual ux_string get_image_decoder_name() = 0;
    virtual ux_string get_image_decoder_extension() = 0;
    virtual std::unique_ptr<gp_image_data> get_decoded_image(ux_string image_name) = 0;
    virtual std::unique_ptr<gp_image_data> get_undecoded_image(ux_string image_name) = 0;
    virtual std::unique_ptr<gp_image_data> decode_image(const gp_image_data& image_to_decode) = 0;
};


#endif //UX_GP_IMAGE_DECODER_H
