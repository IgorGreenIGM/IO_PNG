#ifndef _IEND_CHUNK_H_INCLUDED_
#define _IEND_CHUNK_H_INCLUDED_

#include <cstdio>
#include <fstream>

/**
 * @brief IEND CHUNK class, CRITICAL.
 * 
 */
class IEND_CHUNK
{
    public :
        IEND_CHUNK();
        ~IEND_CHUNK();
        
        void save(std::ofstream &outputStream);

    private : 
        int m_length; /**< the length of the CHUNK */
        uint8_t *m_type = nullptr; /**< the type of the CHUNK corresponding to the name of the chunk in hexadecimal*/
        unsigned long m_crc32; /**< the crc32 value computed from the concatened buffers of type and datas*/
    
    friend class PNG;
};

#endif // _IEND_CHUNK_H_INCLUDED_