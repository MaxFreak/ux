//
// Created by Bernd Lappas on 09.12.15.
//

#ifndef UX_GP_IMAGE_DATA_H
#define UX_GP_IMAGE_DATA_H

#include "ux_types_and_defs.h"

/// The number of entries in the ImageType_t enum. It's very important to set this always to the right number of entries!
const ux_uint nof_image_types = 13;

class gp_image_data
{
public:
    gp_image_data();
    ~gp_image_data();
public:
    /** Enumeration image_type for the loaded format.
        A = alpha, R = red, G = green, B = blue. The order of the letters in the abbreviation defines the order of a pixel.
        The number means the number of bits of each color channel.
        For example: RGBA_8888 means Red, Green, Blue, Alpha and every channel has 8 bits. So
        the whole pixel has 32 bits. Red has the most significant bit and alpha the least siginficant bits.*/
    enum image_type
    {
        IMAGE_FORMAT=0, // Load in the original format of the image.

        // Trick: We use the upper byte to indicate how many bytes are used per pixel.
        // 1 byte per pixel.
            ALPHA_8 = 1<<24, // 8-bit gray

        // 2 bytes per pixel.
            RGB_565 = 2<<24,
        BGR_565,
        ABGR_4444,
        BGRA_4444,

        // 3 bytes per pixel.
            RGB_888 = 3<<24,
        BGR_888,

        // 4 bytes per pixel.
            ARGB_8888 = 4<<24,
        ABGR_8888,
        RGBA_8888,
        BGRA_8888,
        RGB_8888

        // When changing the number of elements be sure to adapt nof_image_types.
    };

    /** Sets the width of the image. Should only be called by an image decoder.
        @param uiWidth Width of the image.
      */
    inline void set_width(const ux_uint &width)                             { m_width = width;}

    /** Sets the height of the image. Should only be called by an image decoder.
        @param uiHeight Height of the image.
      */
    inline void set_height(const ux_uint &height)                           {m_height = height;}

    /** Sets the data pointer of the image. Should only be called by an image
        decoder.
        @param *pubData Pointer to memory to be used for storing a decoded image.
      */
    inline void set_image_data(ux_ubyte *image_data)                                     {m_image_data = image_data;}

    /** Sets the 'filler' value. It is typically used by decoders that load
        non-alpha images but store it in an alpha-enabled format. This
        'filler' is then used as the alpha value.
        This method should be called before CGUIImageDecoder::LoadImg().
        @param ubFillerByte Value used for the alpha filler.
      */
    inline void set_filler(const ux_ubyte &filler_byte) {m_filler_byte = filler_byte;}

    /** Sets the start point of the data.
        This method should be called before CGUIImageDecoder::LoadImg().
        @param bStartTopLeft If true, the first byte is assumed to be the top
               left corner of the image.
      */
    inline void set_start_top_left(const ux_bool &start_top_left) {m_start_top_left = start_top_left;}

    /** Sets the image format type.
        @param eImageType The format type.
      */
    inline void set_image_type(const image_type &image_type) {m_image_type = image_type;}

    /** Sets the destructor's behaviour.
        @param bDeleteDataOnDestruction Pass true to have the destructor delete
               the decoded image data.
      */
    inline void set_delete_data_on_destruction(const ux_bool delete_data) {m_delete_data = delete_data;}

    /// @return The width of the image.
    inline ux_uint get_width() const {return m_width;}

    /// @return The height of the image.
    inline ux_uint get_height() const {return m_height;}

    /// @return The format type of the image.
    inline image_type get_image_type() const {return m_image_type;}

    /** @return Value of the 'filler' byte.
        For an explanation of this value, see SetFiller().
      */
    inline ux_ubyte get_filler() const {return m_filler_byte;}

    /// @return Pointer to the decoded image data.
    inline ux_ubyte* get_image_data() const {return m_image_data;}

    /// @return The bit depth of the image.
    inline ux_ubyte get_bytes_per_pixel() const {return (m_image_type>>24);}

    /// @return True if the first byte of the image data is the top left corner.
    inline ux_bool get_start_top_left() const {return m_start_top_left;}

    /// @return True if this image data deletes its contents upon destruction.
    inline ux_bool get_delete_data_on_destruction() const {return m_delete_data;}

    /** Returns the size (in bytes) of this image.
        Width, height and bits per pixel must have been set correctly before.
        @return Image size in bytes.
    */
    inline ux_uint get_image_size() const { return m_width * m_height * (m_image_type>>24); }

    /** Returns whether the image has alpha channel or not.
        @return True if the image has alpha data, otherwise False.
    */
    ux_bool has_alpha() const;

    /** Deletes the image data array. */
    void delete_data();

    /** Operator= method. This does a flat copy of the given source gp_image_data object, which means that both
        GUIImageData objects will then reference the SAME DATA in memory.
        @param kSource Source object to be copied.
        @return This instance.
    */
    gp_image_data& operator=(const gp_image_data& kSource)
    {
        m_width= kSource.m_width;
        m_height= kSource.m_height;
        m_image_type= kSource.m_image_type;
        m_filler_byte= kSource.m_filler_byte;
        m_image_data= kSource.m_image_data;
        m_start_top_left= kSource.m_start_top_left;
        m_delete_data= kSource.m_delete_data;
        return (*this);
    }

private:
    /** Copy-constructor. Do not use.
        Dummy declaration without implementation, just to hide it.
        @param kSource Source object to be copied.
    */
    gp_image_data(const gp_image_data& kSource);

    ux_uint                      m_width;         ///< Width.
    ux_uint                      m_height;        ///< Height.
    image_type                   m_image_type;      ///< Format.

    /// Filler byte replaces the alpha value, if you load an image with alpha, but you don't want to use it.
    ux_ubyte                     m_filler_byte;

    ux_ubyte*                    m_image_data;         ///< Image data.
    ux_bool                      m_start_top_left;     ///< Is starting point top left or bottom.
    ux_bool                      m_delete_data;        ///< should the data be deleted on destruction?
};

#endif //UX_GP_IMAGE_DATA_H
