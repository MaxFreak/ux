//
// Created by Bernd Lappas on 09.12.15.
//

#include <fstream>
#include <vector>

#include "gp_image_decoder_png.h"

//void ReadPNGData(png_structp png_ptr, png_bytep data, png_size_t length)
//{
//    eC_File* pkTmpImgFile=static_cast<eC_File*>(png_ptr->io_ptr);
//
//    if(pkTmpImgFile != NULL)
//    {
//        pkTmpImgFile->Read8(data, length);
//    }
//    else
//    {
//        GUILOG(GUI_TRACE_WARNING, "CGUIImageLoader::ReadPNGData: Trying to read data of uninitialized file.\n");
//    }
//}

void user_read_data(png_structp pngPtr, png_bytep data, png_size_t length)
{
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    png_voidp a = png_get_io_ptr(pngPtr);
    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    ((std::istream*)a)->read((char*)data, length);
}

std::unique_ptr<gp_image_data> gp_image_decoder_png::get_decoded_image(ux_string image_name)
{
    png_byte pbSig[8];
    double dGamma;
    png_color_16 *pBackground;
    // Since the background color leads to invalid images when contained in a
    // png, the feature is deactivated because the image can't be displayed properly.
    //png_color BkgColor;
    //png_color *pBkgColor = &BkgColor;
    ux_uint width;
    ux_uint height;
    ux_int bit_depth;
    ux_int color_type;
    std::unique_ptr<gp_image_data> image_data (new gp_image_data);

    std::ifstream ifs ("test.txt", std::ifstream::binary);

    char c = ifs.get();

    while (ifs.good()) {
        std::cout << c;
        c = ifs.get();
    }

    ifs.close();

    std::ifstream is ("test.txt", std::ifstream::binary);
    if (is)
    {
        // get length of file:
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

//        char *buffer = new char[length];
//
//        std::cout << "Reading " << length << " characters... ";
//        // read data as a block:
//        is.read(buffer, length);
//
//        if (is)
//            std::cout << "all characters read successfully.";
//        else
//            std::cout << "error: only " << is.gcount() << " could be read";
//        is.close();
//
//        // ...buffer contains the entire file...
//
//        delete[] buffer;
    }

//    pTempImageFile->Reset();

    // Read file signature.
//    pTempImageFile->Read8(pbSig, 8);
    is.read((char*)pbSig, 8);

    // Check if the file is a png file.
    if (!png_check_sig(pbSig, 8))
    {
        return nullptr;
    }

    png_structp png_ptr;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
        return nullptr;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return nullptr;
    }

    // prepare for errors.
    if (setjmp(png_jmpbuf(png_ptr)))  // png_jmpbuf is a macro in pngconf.h
    {
        UX_TRACE_ERROR("gp_image_decoder_png::get_decoded_image: libPNG error, png_destroy_read_struct() will be called.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return nullptr;
    }

    // Set read function for use with eC_File.
    png_set_read_fn(png_ptr,(png_voidp)&is, user_read_data);
//    png_set_read_fn(png_ptr, static_cast<png_voidp>(is), static_cast<png_rw_ptr>(user_read_data));

    // We already read the image signature.
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // Read image attributes.
    png_get_IHDR(png_ptr, info_ptr, static_cast<png_uint_32*>(&width), static_cast<png_uint_32*>(&height), &bit_depth,
                 &color_type, NULL, NULL, NULL);

    // to be able to load pngs with 16Bits (two bytes) per color channel, strip 16 bit samples to 8 bit.
    if (bit_depth == 16)
    {
        png_set_strip_16(png_ptr);
    }

    image_data->set_width(width);
    image_data->set_height(height);

    if (image_data->get_image_type() != gp_image_data::IMAGE_FORMAT) // we have to load the image with format transformations
    {
        switch(color_type)
        {
            case PNG_COLOR_TYPE_RGB_ALPHA:
                switch (image_data->get_image_type())
                {
                    case gp_image_data::ARGB_8888:
                        png_set_swap_alpha(png_ptr);
                        break;
                    case gp_image_data::ABGR_8888:
                        png_set_swap_alpha(png_ptr);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGBA_8888:
                        // nothing to do
                        break;
                    case gp_image_data::BGRA_8888:
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGB_888:
                        png_set_strip_alpha(png_ptr);
                        break;
                    case gp_image_data::BGR_888:
                        png_set_strip_alpha(png_ptr);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::ALPHA_8:
                        png_set_strip_alpha(png_ptr);
                        png_set_rgb_to_gray_fixed(png_ptr,1,21268,71514);
                        break;
                        //case RGB_565:
                        //    png_set_strip_16(png_ptr);
                        //    png_ptr->transformations(
                        //    break;
                        //case BGR_565:
                        //    break;
                        //case ABGR_4444:
                        //    break;
                        //case BGRA_4444:
                        //    break;
                    default:
                    UX_THROW_EXCEPTION(ux_exception(),"gp_image_decoder_png::get_decoded_image: Unknown destination format.\n");
                        break;
                }
                break;
            case PNG_COLOR_TYPE_RGB:
                switch (image_data->get_image_type())
                {
                    case gp_image_data::ARGB_8888:
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        break;
                    case gp_image_data::ABGR_8888:
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGBA_8888:
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        break;
                    case gp_image_data::BGRA_8888:
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGB_888:
                        break;
                    case gp_image_data::BGR_888:
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::ALPHA_8:
                        png_set_strip_alpha(png_ptr);
                        png_set_rgb_to_gray_fixed(png_ptr,1,21268,71514);
                        break;
                        //case RGB_565:
                        //    break;
                        //case BGR_565:
                        //    break;
                        //case ABGR_4444:
                        //    break;
                        //case BGRA_4444:
                        //    break;
                    default:
                        UX_THROW_EXCEPTION(ux_exception(),"CGUIImageDecoderPNG::LoadImg: Unknown destination format.\n");
                        break;
                }
                break;
            case PNG_COLOR_TYPE_GRAY:
                switch (image_data->get_image_type())
                {
                    case gp_image_data::ARGB_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        break;
                    case gp_image_data::ABGR_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGBA_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        break;
                    case gp_image_data::BGRA_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGB_888:
                        png_set_gray_to_rgb(png_ptr);
                        break;
                    case gp_image_data::BGR_888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::ALPHA_8:
                        png_set_strip_alpha(png_ptr);
                        break;
                        //case RGB_565:
                        //    break;
                        //case BGR_565:
                        //    break;
                        //case ABGR_4444:
                        //    break;
                        //case BGRA_4444:
                        //    break;
                    default:
                        UX_THROW_EXCEPTION(ux_exception(),"CGUIImageDecoderPNG::LoadImg: Unknown destination format.\n");
                        break;
                }
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                switch (image_data->get_image_type())
                {
                    case gp_image_data::ARGB_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        break;
                    case gp_image_data::ABGR_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGBA_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        break;
                    case gp_image_data::BGRA_8888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGB_888:
                        png_set_gray_to_rgb(png_ptr);
                        break;
                    case gp_image_data::BGR_888:
                        png_set_gray_to_rgb(png_ptr);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::ALPHA_8:
                        png_set_strip_alpha(png_ptr);
                        break;
                        //case RGB_565:
                        //    break;
                        //case BGR_565:
                        //    break;
                        //case ABGR_4444:
                        //    break;
                        //case BGRA_4444:
                        //    break;
                    default:
                    UX_THROW_EXCEPTION(ux_exception(),"CGUIImageDecoderPNG::LoadImg: Unknown destination format.\n");
                        break;
                }
                break;
            case PNG_COLOR_TYPE_PALETTE:
                switch (image_data->get_image_type())
                {
                    case gp_image_data::ARGB_8888:
                        png_set_palette_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        break;
                    case gp_image_data::ABGR_8888:
                        png_set_palette_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_BEFORE);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGBA_8888:
                        png_set_palette_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        break;
                    case gp_image_data::BGRA_8888:
                        png_set_palette_to_rgb(png_ptr);
                        png_set_filler(png_ptr, image_data->get_filler(), PNG_FILLER_AFTER);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::RGB_888:
                        png_set_palette_to_rgb(png_ptr);
                        break;
                    case gp_image_data::BGR_888:
                        png_set_palette_to_rgb(png_ptr);
                        png_set_bgr(png_ptr);
                        break;
                    case gp_image_data::ALPHA_8:
                        png_set_palette_to_rgb(png_ptr);
                        png_set_strip_alpha(png_ptr);
                        png_set_rgb_to_gray_fixed(png_ptr,1,21268,71514);
                        break;
                        //case RGB_565:
                        //    break;
                        //case BGR_565:
                        //    break;
                        //case ABGR_4444:
                        //    break;
                        //case BGRA_4444:
                        //    break;
                    default:
                    UX_THROW_EXCEPTION(ux_exception(),"CGUIImageDecoderPNG::LoadImg: Unknown destination format.\n");
                        break;
                }
                break;
            default:
                ux_string kError = "CGUIImageDecoderPNG::LoadImg: Unknown PNG color type (with transformation: " +
                                                 ux_string(std::to_string(color_type)) + "\n";
                UX_TRACE_ERROR(kError);
                return nullptr;
                break;
        }
    }
    else // We load the image without transformation.
    {
        // Set the image type to the original format.
        switch(color_type)
        {
            case PNG_COLOR_TYPE_RGB_ALPHA:
                image_data->set_image_type(gp_image_data::RGBA_8888);
                break;
            case PNG_COLOR_TYPE_RGB:
                image_data->set_image_type(gp_image_data::RGB_888);
                break;
            case PNG_COLOR_TYPE_GRAY:
                image_data->set_image_type(gp_image_data::ALPHA_8);
                break;
            case PNG_COLOR_TYPE_PALETTE:
                UX_TRACE_ERROR("CGUIImageDecoderPNG::LoadImg: Unhandled PNG color type PNG_COLOR_TYPE_PALETTE (without transformation)\n");
                return nullptr;
                break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                UX_TRACE_ERROR("CGUIImageDecoderPNG::LoadImg: Unhandled PNG color type PNG_COLOR_TYPE_GRAY_ALPHA (without transformation)\n");
                return nullptr;
                break;
            default:
                ux_string kError = ux_string("CGUIImageDecoderPNG::LoadImg: Unknown PNG color type (without transformation): " +
                                                 ux_string(std::to_string(color_type)) + "\n");
                UX_TRACE_ERROR(kError);
                return nullptr;
                break;
        }
    }

    // Set the background color to draw transparent and alpha images over.
    if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
    {
        //setting the background image color (if set in the png)
        //leads to invalid image data (image can't be displayed correctly).
        //Since Guiliani does not use this feature on any platform,
        //it is deactivated.
        UX_TRACE_DEBUG("CGUIImageDecoderPNG::LoadImg: Found background color in png information."
            "The background color is not used by this decoder.\n");

        //png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
        //pBkgColor->red = (unsigned char) pBackground->red;
        //pBkgColor->green = (unsigned char) pBackground->green;
        //pBkgColor->blue = (unsigned char) pBackground->blue;
    }
    //else
    //{
    //    pBkgColor = NULL;
    //}

    // If required set gamma conversion.
    if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
    {
        png_set_gamma(png_ptr, static_cast<double>(2.2), dGamma);
    }
    // After the transformations have been registered update info_ptr data.
    png_read_update_info(png_ptr, info_ptr);

    // Get again width, height and the new bit-depth and color-type.
    png_get_IHDR(png_ptr, info_ptr, static_cast<png_uint_32*>(&width), static_cast<png_uint_32*>(&height), &bit_depth,
                 &color_type, NULL, NULL, NULL);

    image_data->set_width(width);
    image_data->set_height(height);

    //The number of bytes in one row of the transformed source PNG as reported by the PNGlib
    ux_uint uiPNGRowbytes = png_get_rowbytes(png_ptr, info_ptr);
    //The number of bytes in one line of the destination image's GUIImageData
    ux_uint uiGUIImageRowbytes = image_data->get_width() * image_data->get_bytes_per_pixel();

    if ( uiPNGRowbytes != uiGUIImageRowbytes)
    {
        ux_string kError = ux_string("CGUIImageDecoderPNG::LoadImg: transformation error: image row size (" +
                                         std::to_string(uiGUIImageRowbytes) + " Bytes) is not equal to PNG's rowbytes value (" +
                                 std::to_string(uiPNGRowbytes) + " Bytes).\n");

        if ( uiPNGRowbytes > uiGUIImageRowbytes)
        {
            UX_TRACE_ERROR(kError);
            return nullptr;
        }
        else
        {
            UX_TRACE_WARNING(kError);
        }
    }

    // Allocate array of row pointers where pnglib writes the data.
    std::vector<png_byte*> ppbRowPointers(image_data->get_height());
//    eC_TArray<png_byte*> ppbRowPointers(image_data->get_height());

    // Allocate memory for the image.
    png_byte* ubImageData = new png_byte[image_data->get_width() * image_data->get_height() * image_data->get_bytes_per_pixel()];

    if (image_data->get_start_top_left())
    {
        for (ux_uint i=0; i < image_data->get_height(); i++)
        {
            // Set the row pointers, so that pnglib writes the data into our image data block.
            ppbRowPointers[i] = ubImageData + (i * image_data->get_bytes_per_pixel() * image_data->get_width());
        }
    }
    else
    {
        ux_uint uiSrc = image_data->get_height()-1;
        for (ux_uint i=0; i < image_data->get_height(); i++)
        {
            // Set the row pointers, so that pnglib writes the data into our image data block.
            ppbRowPointers[i] = ((png_byte*)(ubImageData) + uiSrc * image_data->get_bytes_per_pixel() * image_data->get_width());
            uiSrc--;
        }
    }

    // Now we can go ahead and just read the whole image.
    png_read_image(png_ptr, ppbRowPointers.data());

    // Read the additional chunks in the PNG file (not really needed).
    png_read_end(png_ptr, NULL);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    // Save image data in gp_image_data object.
    image_data->set_image_data(ubImageData);

    is.close();
    return nullptr;
}

std::unique_ptr<gp_image_data> gp_image_decoder_png::decode_image(const gp_image_data& image_to_decode)
{
    return nullptr;
}
