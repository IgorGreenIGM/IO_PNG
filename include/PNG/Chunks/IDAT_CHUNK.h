#ifndef _IDAT_CHUNK_H_INCLUDED_
#define _IDAT_CHUNK_H_INCLUDED_

#include <cstdio>
#include <thread>
#include <future>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>


/**
 * @brief IDAT CHUNK class, CRITICAL.
 * 
 */
class IDAT_CHUNK
{   
    public :
        IDAT_CHUNK(const uint8_t *pixelsBuffer, int s_width, int s_height, int colorChannel);
        ~IDAT_CHUNK();
        
        void save(std::ofstream &outputStream);

    private : 
        int m_length; /**< the length of the CHUNK */
        uint8_t *m_type = nullptr; /**< the type of the CHUNK corresponding to the name of the chunk in hexadecimal*/
        uint8_t *m_data = nullptr; /**< the datas inside the CHUNK, coresponding to the deflated scanlines generated from the input pixels buffer*/
        uint8_t *pixelsBuffer = nullptr; /**< the input pixels buffer*/
        unsigned long m_crc32; /**< the crc32 value computed from the concatened buffers of type and datas*/

        uint8_t *generate_scanlines(const uint8_t *pixelBuffer, int s_width, int s_height, int colorChannel);
        uint8_t *deflate_datas(const uint8_t *pixelBuffer, int s_width, int s_height, int colorChannel, int &deflatedLen);
        uint8_t *filter_line(const uint8_t *line_in, int lineLength, uint8_t filterMode, bool is_prev_line, const uint8_t *unfiltered_prev_line, uint8_t colorChannel);

    friend class PNG;
};

#endif // _IDAT_CHUNK_H_INCLUDED_