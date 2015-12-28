//
// Created by Bernd Lappas on 09.12.15.
//

#ifndef UX_GP_IMAGE_DECODER_PNG_H
#define UX_GP_IMAGE_DECODER_PNG_H

#include "ux_types_and_defs.h"
#include "gp_image_decoder.h"

#include "png.h"

class gp_image_decoder_png : public gp_image_decoder
{
public:
    gp_image_decoder_png():gp_image_decoder() {};
    virtual ~gp_image_decoder_png(){};
    virtual ux_string get_image_decoder_name() { return "png images";};
    virtual ux_string get_image_decoder_extension() {return "png";};
    virtual std::unique_ptr<gp_image_data> get_decoded_image(ux_string image_name);
    virtual std::unique_ptr<gp_image_data> get_undecoded_image(ux_string image_name) { return nullptr;};
    virtual std::unique_ptr<gp_image_data> decode_image(const gp_image_data& image_to_decode);
};

#endif //UX_GP_IMAGE_DECODER_PNG_H
