//
// Created by Bernd Lappas on 09.12.15.
//

#include "gp_image_loader.h"

std::string GetFileExtension(const std::string& FileName)
{
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".")+1);
    return "";
}

std::unique_ptr<gp_image_data> gp_image_loader::load_image(const ux_string &file_path)
{
    for (auto &decoder: m_decoder_vector)
    {
        ux_string dec_ext = decoder.get().get_image_decoder_extension();
        if (GetFileExtension(file_path) == dec_ext)
            return decoder.get().get_decoded_image(file_path);
    }
    return nullptr;
}

void gp_image_loader::add_decoder(gp_image_decoder &image_decoder)
{
    m_decoder_vector.push_back(std::ref(image_decoder));
}
