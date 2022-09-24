#ifndef _IHDR_CHUNK_H_INCLUDED_
#define _IHDR_CHUNK_H_INCLUDED_

#include <cstdio>
#include <fstream>


/**
 * @brief IHDR CHUNK class, CRITICAL.
 * 
 */
class IHDR_CHUNK
{
    public  :
        IHDR_CHUNK(int width, int height, int bitDepth, int colorMode);
        ~IHDR_CHUNK();

        int get_width();
        int get_height();
        uint8_t get_bitDepth();
        uint8_t get_colorMode();
        uint8_t get_interlacing();

        void save(std::ofstream &outputStream);

    private : 
        int m_length; /**< the length of the CHUNK */
        int m_width;  /**< the width of the PNG file */ 
        int m_height; /**< the height of the PNG file */
        uint8_t *m_type = nullptr; /**< the type of the CHUNK corresponding to the name of the chunk in hexadecimal*/
        uint8_t *m_data = nullptr; /**< the datas inside the CHUNK*/
        uint8_t *pixelsBuffer = nullptr; /**< the input pixels buffer*/
        unsigned long m_crc32; /**< the crc32 value computed from the concatened buffers of type and datas*/
    
    friend class PNG;
};

#endif // _IHDR_CHUNK_H_INCLUDED_