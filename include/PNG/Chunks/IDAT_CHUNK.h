#ifndef _IDAT_CHUNK_H_INCLUDED_
#define _IDAT_CHUNK_H_INCLUDED_

#include <cstdio>
#include <fstream>

/**
 * @brief IDAT CHUNK class, CRITICAL.
 * 
 */
class IDAT_CHUNK
{   
    public :
        IDAT_CHUNK(uint8_t *pixelsBuffer, int s_width, int s_height, int colorChannel);
        ~IDAT_CHUNK();
        void save(std::ofstream &outputStream);

        static uint8_t *generateScanlines(uint8_t *pixelBuffer, int s_width, int s_height, int colorChannel);
        static uint8_t *deflateDatas(uint8_t *pixelBuffer, int s_width, int s_height, int colorChannel, int &deflatedLen);

    private : 
        int m_length; /**< the length of the CHUNK */
        uint8_t *m_type = nullptr; /**< the type of the CHUNK corresponding to the name of the chunk in hexadecimal*/
        uint8_t *m_data = nullptr; /**< the datas inside the CHUNK, coresponding to the deflated scanlines generated from the input pixels buffer*/
        uint8_t *pixelsBuffer = nullptr; /**< the input pixels buffer*/
        unsigned long m_crc32; /**< the crc32 value computed from the concatened buffers of type and datas*/
};

#endif //_IDAT_CHUNK_H_INCLUDED_